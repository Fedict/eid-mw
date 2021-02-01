#ifndef __quarantine_h__
#define __quarantine_h__

#include <stdint.h>

typedef struct _qtn_file_s *qtn_file_t;
typedef struct qtn_proc * qtn_proc_t;

#ifndef QTN_FLAG_SANDBOX
#define QTN_FLAG_SANDBOX 1
#endif /* QTN_FLAG_SANDBOX */

#ifndef QTN_FLAG_HARD
#define QTN_FLAG_HARD 2
#endif /* QTN_FLAG_HARD */

extern qtn_proc_t _qtn_proc_alloc(void);
extern void _qtn_proc_set_identifier(qtn_proc_t qp, char *identifier);
extern void _qtn_proc_set_flags(qtn_proc_t qp, int flags);
extern void _qtn_proc_apply_to_self(qtn_proc_t qp);
extern void _qtn_proc_free(qtn_proc_t qp);

extern qtn_file_t _qtn_file_alloc(void);
extern void _qtn_file_free(qtn_file_t);

extern qtn_file_t _qtn_file_clone(qtn_file_t);

extern int _qtn_file_init_with_fd(qtn_file_t file, int fd);
extern int _qtn_file_apply_to_fd(qtn_file_t file, int fd);

extern int _qtn_file_init_with_path(qtn_file_t file, const char *path);

extern int _qtn_file_init_with_data(qtn_file_t file, const void *, size_t);
extern int _qtn_file_to_data(qtn_file_t file, void *, size_t*);

extern int _qtn_file_apply_to_path(qtn_file_t qf, char *path);

extern const char *_qtn_error(int err);

extern const char *_qtn_xattr_name;

extern const int _qtn_proc_to_data(void * spawn_quarantine, char *qbuf,size_t *qbuf_sz);

#define QTN_SERIALIZED_DATA_MAX 4096

#define qtn_file_alloc _qtn_file_alloc
#define qtn_file_free _qtn_file_free

#define qtn_file_clone _qtn_file_clone

#define qtn_file_init_with_fd _qtn_file_init_with_fd
#define qtn_file_apply_to_fd _qtn_file_apply_to_fd

#define qtn_file_init_with_path _qtn_file_init_with_path

#define qtn_file_init_with_data _qtn_file_init_with_data
#define qtn_file_to_data _qtn_file_to_data

#define qtn_error _qtn_error

#define qtn_xattr_name _qtn_xattr_name

#define qtn_proc_to_data _qtn_proc_to_data

#endif
