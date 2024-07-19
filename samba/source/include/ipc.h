#ifndef __RPC_PIPE_API_H__
#define __RPC_PIPE_API_H__

/*
  header for account policy
*/
#ifndef RPC_PIPE_API
#define reset_chain_p() do{}while(0)
#define pipe_close_conn(...) do{}while(0)
#define init_rpc_pipe_hnd() do{}while(0)
#define set_pipe_handle_offset(...) do{}while(0)
#define get_first_internal_pipe(...) ({NULL;})
#define get_next_internal_pipe(...) ({NULL;})
#define set_rpc_pipe_hnd_state(...) ({False;})
#define wait_rpc_pipe_hnd_state(...) ({False;})
#define write_to_pipe(...) ({-1;})
#define read_from_pipe(...) ({-1;})
#define reply_open_pipe_and_X(...) ({ERROR_DOS(ERRSRV,ERRnosupport);})
#define reply_pipe_write(...) ({ERROR_DOS(ERRSRV,ERRnosupport);})
#define reply_pipe_write_and_X(...) ({ERROR_DOS(ERRSRV,ERRnosupport);})
#define reply_pipe_read_and_X(...) ({ERROR_DOS(ERRSRV,ERRnosupport);})
#define reply_pipe_close(...) ({ERROR_DOS(ERRSRV,ERRnosupport);})
#else
void reset_chain_p(void);
void pipe_close_conn(connection_struct *conn);
void init_rpc_pipe_hnd(void);
void set_pipe_handle_offset(int max_open_files);
pipes_struct *get_first_internal_pipe(void);
pipes_struct *get_next_internal_pipe(pipes_struct *p);
BOOL set_rpc_pipe_hnd_state(smb_np_struct *p, uint16 device_state);
BOOL wait_rpc_pipe_hnd_state(smb_np_struct *p, uint16 priority);
ssize_t write_to_pipe(smb_np_struct *p, char *data, size_t n);
ssize_t read_from_pipe(smb_np_struct *p, char *data, size_t n, BOOL *is_data_outstanding);
int reply_open_pipe_and_X(connection_struct *conn, char *inbuf,char *outbuf,int length,int bufsize);
int reply_pipe_write(char *inbuf,char *outbuf,int length,int dum_bufsize);
int reply_pipe_write_and_X(char *inbuf,char *outbuf,int length,int bufsize);
int reply_pipe_read_and_X(char *inbuf,char *outbuf,int length,int bufsize);
int reply_pipe_close(connection_struct *conn, char *inbuf,char *outbuf);
#endif

int reply_trans(connection_struct *conn, char *inbuf,char *outbuf,
		int size, int bufsize);
int reply_transs(connection_struct *conn, char *inbuf,char *outbuf,
		 int size, int bufsize);

#endif	/* _IPC_H_ */
