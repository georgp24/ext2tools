/*
 * com_right.c -- provide Heimdall / Kerberos4kth com_err interfaces
 * 	for backwards compatbility 
 * 
 * Copyright (c) 2003 by Theodore Ts'o
 *
 * Taken from lib/com_err/error.c from Kerberos4kth distribution.
 * 
 * Copyright (c) 1997, 1998, 2001 Kungliga Tekniska H�gskolan
 * (Royal Institute of Technology, Stockholm, Sweden). 
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "com_err.h"
#include "error_table.h"

const char *
com_right(struct et_list *list, long code)
{
    struct et_list *p;
    for (p = list; p; p = p->next) {
	if (code >= p->table->base && code < p->table->base + p->table->n_msgs)
	    return p->table->msgs[code - p->table->base];
    }
    return NULL;
}

struct foobar {
    struct et_list etl;
    struct error_table tab;
};

/*
 * We provide this routine for compatibility with Heimdall generated
 * foo_err.c files, but we don't use this ourselves for foo_err.c
 * files generated by our compile_et.  This is so our foo_err.c
 * files can be used with older com_err libraries without running
 * afoul of dependencies.
 */
void
initialize_error_table_r(struct et_list **list, 
			 const char **messages, 
			 int num_errors,
			 long base)
{
    struct et_list *et, **end;
    struct error_table *tab;
    struct foobar *f;

    for (end = list, et = *list; et; end = &et->next, et = et->next)
        if (et->table->msgs == messages)
            return;
    f = malloc(sizeof(*f));
    if (f == NULL)
        return;
    et = &f->etl;
    et->table = tab = &f->tab;
    tab->msgs = messages;
    tab->n_msgs = num_errors;
    tab->base = base;
    et->next = NULL;
    *end = et;
}
			

void
free_error_table(struct et_list *et)
{
    while(et){
	struct et_list *p = et;
	et = et->next;
	free(p);
    }
}
