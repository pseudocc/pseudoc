#ifndef PSEUDOC_KERNEL_H
#define PSEUDOC_KERNEL_H

typedef enum psignal_e psignal_t;

enum psignal_e {
  SIGN_NOTIMPL,
  SIGN_OUTOFMEM,
  SIGN_SEGFAULT,
  SIGN_ABORT,
  SIGN_QUIT,
  SIGN_EXCEPTION
};

void pkinit(char** args, int argc);
void pkrun();

int pkexit();
void pkraise(psignal_t s);

#endif
