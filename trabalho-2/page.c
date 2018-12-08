#include <stdio.h>
#include "page.h"
#include "util.h"

#define TRUE 1
#define FALSE 0

struct page {
    /* Índice do quadro de memória. */
    int page_frame;

    /* Indica se está presenta na memória física. */
    int present;

    /* Indica se a página foi modificada. */
    int modified;

    /* Indica se a página foi referenciada. */
    int referenced;
};

Page *create_page() {
    Page *new = (Page *) _malloc(sizeof(Page));
    new->page_frame = -1;
    new->present = FALSE;
    new->modified = FALSE;
    new->referenced = FALSE;

    return new;
}

void allocate_page(Page *page, int page_frame) {
    page->page_frame = page_frame;
    page->present = TRUE;
    page->modified = FALSE;
    page->referenced = FALSE;
}

void deallocate_page(Page *page) {
    page->page_frame = -1;
    page->present = FALSE;
}

void set_modified(Page *page) {
    page->modified = TRUE;
}

void set_referenced(Page *page) {
    page->referenced = TRUE;
}

int get_page_frame(Page *page) {
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

