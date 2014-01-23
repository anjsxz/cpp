#include "uv.h"
#include<stdlib.h>
#include <stdio.h>
uv_loop_t *loop;
uv_udp_send_t send_socket;
uv_udp_t recv_socket;
uv_alloc_cb alloc_buffer;
//uv_udp_recv_cb on_read;
void send_cb(uv_udp_send_t* req, int status) {
    if (status != 0) {
        uv_err_t err = uv_last_error(loop);
        fprintf(stderr, "send_cb error: %s\n", uv_strerror(err));
    }
    
    //uv_buf_t *send_buf = (uv_buf_t *)req.bufs;//req.bufs is NULL pointer
    //free(send_buf->base);
    //free(send_buf.base);
    //free(req->data);
    //free(req->handle);
    //uv_close((uv_handle_t*) req->handle, NULL );
    //free(req->bufs[0].base);
    free(req);
}
void on_recv(uv_udp_t *res, ssize_t nread, uv_buf_t buf, struct sockaddr *addr, unsigned flags) {
    if (nread == -1) {
        fprintf(stderr, "Read error %s\n", uv_err_name(uv_last_error(loop)));
        uv_close((uv_handle_t*) res, NULL);
        free(buf.base);
        return;
    }
    
    char sender[512] = { 0 };
    uv_ip4_name((struct sockaddr_in*) addr, sender, 16);
    fprintf(stderr, "Recv from %s\n", sender);
    uv_udp_t handle;
    uv_buf_t bufs[1];
    //UV_EXTERN int uv_udp_send(uv_udp_send_t* req, uv_udp_t* handle,
//    uv_buf_t bufs[], int bufcnt, struct sockaddr_in addr,
//    uv_udp_send_cb send_cb);
    uv_udp_send(&send_socket, &handle, bufs, 1, *(struct sockaddr_in*)addr,  send_cb);
    
    free(buf.base);
    uv_udp_recv_stop(res);
}

int main() {
    loop = uv_default_loop();
    
    uv_udp_init(loop, &recv_socket);
    struct sockaddr_in recv_addr = uv_ip4_addr("127.0.0.1", 6000);
    uv_udp_bind(&recv_socket, recv_addr, 0);
    uv_udp_recv_start(&recv_socket, alloc_buffer, on_recv);
    
   
    return uv_run(loop, UV_RUN_DEFAULT);
}