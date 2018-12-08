typedef struct page Page;

Page *create_page();

void allocate_page(Page *page, int page_frame);

void deallocate_page(Page *page);

void set_modified(Page *page, int modified);

void set_referenced(Page *page, int referenced);

int get_page_frame(Page *page);

int get_present(Page *page);

int get_modified(Page *page);

int get_referenced(Page *page);

