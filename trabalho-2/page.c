#include <stdio.h>
#include "page.h"
#include "util.h"

#define TRUE 1
#define FALSE 0

struct page {
    /* Índice do quadro de memória. */
    unsigned int page_frame;

    /* Indica se está presenta na memória física. */
    int present;

    /* Indica se a página foi modificada. */
    int modified;

    /* Indica se a página foi referenciada. */
    int referenced;

    /* Instante de último acesso. */
    int last_access;
};

Page *create_page() {
    Page *new = (Page *) _malloc(sizeof(Page));
    new->page_frame = 0;
    new->present = FALSE;
    new->modified = FALSE;
    new->referenced = FALSE;

    return new;
}

void allocate_page(Page *page, unsigned int page_frame) {
    page->page_frame = page_frame;
    page->present = TRUE;
}

void deallocate_page(Page *page) {
    page->page_frame = -1;
    page->present = FALSE;
    if(page->modified) {
	// "salva no disco" e reseta bit M.
	page->modified = FALSE;
    }
}

void set_modified(Page *page, int modified) {
    page->modified = modified;
}

void set_referenced(Page *page, int referenced) {
    page->referenced = referenced;
}

void set_last_access(Page *page, unsigned int ts) {
    page->last_access = ts;
}

unsigned int get_page_frame(Page *page) {
    return page->page_frame; 
}

int get_present(Page *page) {
    return page->present;
}

int get_modified(Page *page) {
    return page->modified;
}

int get_referenced(Page *page) {
    return page->referenced;
}

unsigned int get_last_access(Page *page) {
    return page->last_access;
}

