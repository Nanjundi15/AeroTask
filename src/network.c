#include "network.h"
#include "scheduler.h"
#include "storage.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>

static scheduler_t scheduler;
static volatile sig_atomic_t stopping=0;

static void on_signal(int sig) { (void)sig; stopping=1; }

static void send_text(int fd, const char *s) {
    send(fd,s,strlen(s),0);
}

static void handle_client(int fd) {
    char buf[AT_MAX_PAYLOAD];
    ssize_t n=recv(fd,buf,sizeof(buf)-1,0);
    if(n<=0) return;
    buf[n]='\0';
    char *cmd=strtok(buf," \r\n");
    if(!cmd){send_text(fd,"ERR empty command\n");return;}

    if(strcasecmp(cmd,"SUBMIT")==0) {
        char *priority_s=strtok(NULL," \r\n");
        char *name=strtok(NULL,"\r\n");
        if(!priority_s || !name) { send_text(fd,"ERR usage: SUBMIT <priority> <name>\n"); return; }
        task_t t={0};
        snprintf(t.name,sizeof(t.name),"%s",name);
        t.priority=atoi(priority_s);
        t.max_retries=AT_DEFAULT_RETRIES;
        if(scheduler_submit(&scheduler,&t)==0) {
            char out[128]; snprintf(out,sizeof(out),"OK task_id=%d status=PENDING\n",t.id); send_text(fd,out);
        } else send_text(fd,"ERR submit failed\n");
    } else if(strcasecmp(cmd,"STATUS")==0) {
        char *id_s=strtok(NULL," \r\n");
        task_t t;
        if(!id_s || storage_get_task(atoi(id_s),&t)!=0) send_text(fd,"ERR task not found\n");
        else {
            char out[512];
            snprintf(out,sizeof(out),"TASK %d | %s | priority=%d | retries=%d/%d | worker=%d | error=%s\n",
                t.id,task_status_name(t.status),t.priority,t.retry_count,t.max_retries,t.worker_id,t.error);
            send_text(fd,out);
        }
    } else if(strcasecmp(cmd,"LIST")==0) {
        /* Listing is printed server-side for simplicity. */
        storage_list_tasks();
        send_text(fd,"OK list printed on server console\n");
    } else if(strcasecmp(cmd,"STATS")==0) {
        storage_print_stats();
        send_text(fd,"OK stats printed on server console\n");
    } else if(strcasecmp(cmd,"HEALTH")==0) {
        char out[128]; snprintf(out,sizeof(out),"OK HEALTH queue=%zu\n",queue_size(&scheduler.queue)); send_text(fd,out);
    } else {
        send_text(fd,"ERR commands: SUBMIT STATUS LIST STATS HEALTH\n");
    }
}

int network_server_run(int port) {
    signal(SIGINT,on_signal);
    signal(SIGTERM,on_signal);
    if(storage_open("data/aerotask.db")!=0){log_error("Storage initialization failed");return 1;}
    if(scheduler_init(&scheduler,4)!=0){storage_close();return 1;}
    scheduler_start(&scheduler);

    int server_fd=socket(AF_INET,SOCK_STREAM,0);
    if(server_fd<0){perror("socket");scheduler_stop(&scheduler);storage_close();return 1;}
    int opt=1; setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    struct sockaddr_in addr={0};
    addr.sin_family=AF_INET; addr.sin_addr.s_addr=INADDR_ANY; addr.sin_port=htons((uint16_t)port);
    if(bind(server_fd,(struct sockaddr*)&addr,sizeof(addr))<0){perror("bind");close(server_fd);scheduler_stop(&scheduler);storage_close();return 1;}
    if(listen(server_fd,16)<0){perror("listen");close(server_fd);scheduler_stop(&scheduler);storage_close();return 1;}

    log_info("AeroTask server listening on port %d",port);
    while(!stopping) {
        struct sockaddr_in client; socklen_t len=sizeof(client);
        int fd=accept(server_fd,(struct sockaddr*)&client,&len);
        if(fd<0){if(errno==EINTR)continue;break;}
        handle_client(fd);
        close(fd);
    }
    close(server_fd);
    scheduler_stop(&scheduler);
    storage_close();
    return 0;
}

int network_client_run(const char *host,int port,int argc,char **argv) {
    int fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd<0){perror("socket");return 1;}
    struct sockaddr_in addr={0};
    addr.sin_family=AF_INET; addr.sin_port=htons((uint16_t)port);
    if(inet_pton(AF_INET,host,&addr.sin_addr)<=0){fprintf(stderr,"Invalid IPv4 address\n");close(fd);return 1;}
    if(connect(fd,(struct sockaddr*)&addr,sizeof(addr))<0){perror("connect");close(fd);return 1;}

    char request[AT_MAX_PAYLOAD]="";
    for(int i=0;i<argc;i++){
        if(i) strncat(request," ",sizeof(request)-strlen(request)-1);
        strncat(request,argv[i],sizeof(request)-strlen(request)-1);
    }
    strncat(request,"\n",sizeof(request)-strlen(request)-1);
    send_text(fd,request);
    char response[1024]; ssize_t n=recv(fd,response,sizeof(response)-1,0);
    if(n>0){response[n]='\0';printf("%s",response);}
    close(fd); return 0;
}
