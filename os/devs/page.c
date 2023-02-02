#include "os.h"

extern uint32_t TEXT_START;
extern uint32_t TEXT_END;
extern uint32_t DATA_START;
extern uint32_t DATA_END;
extern uint32_t RODATA_START;
extern uint32_t RODATA_END;
extern uint32_t BSS_START;
extern uint32_t BSS_END;
extern uint32_t HEAP_START;
extern uint32_t HEAP_SIZE;

static uint32_t _alloc_start = 0;
static uint32_t _alloc_end = 0;
static uint32_t _num_pages = 0;

#define PAGE_SIZE 4096
#define PAGE_ORDER 12

#define TO_TAKEN_PAGE (uint8_t)(1 << 0)
#define TO_LAST_PAGE  (uint8_t)(1 << 1)
#define RESERVERED_NUM 8

typedef struct PageTableElement {
	uint8_t flags;
} PTE;


static inline void _clear(PTE *pte)
{
	pte->flags = 0;
}

static inline void _bit_init(PTE *vtp, uint8_t init_bit)
{
	vtp->flags = init_bit;
}

static inline int _is_free(PTE *pte)
{
	if (pte->flags & TO_TAKEN_PAGE) {
		return 0;
	} else {
		return 1;	
	}
}

static inline int _is_last(PTE *pte)
{
	if (pte->flags & TO_LAST_PAGE) {
		return 1;
	} else {
		return 0;
	}
}

static inline void _set_flag(PTE *pte, uint8_t new_flags)
{
	pte->flags |= new_flags;
}

static inline uint32_t _align_page(uint32_t address)
{
	uint32_t order = (1 << PAGE_ORDER) - 1;
	return (address + order) & (~order);
}


void page_init()
{

	/**
	 *  遍历页表
	 *  对于每一个页表项，都是直接通过物理地址访问（没有做虚拟地址） 
	 *	每一个页表项，我们都是初始化成全0b00000001.
	 * HEAP_START开始一段是页表   
	 */
	_num_pages = (HEAP_SIZE / PAGE_SIZE) - RESERVERED_NUM;
	printf("HEAP_START = %x, HEAP_SIZE = %x, num of pages = %d\n", 
				HEAP_START, HEAP_SIZE, _num_pages);
	PTE *pte_addr = (PTE *)HEAP_START; // pte_add指向一个pte,我们每次pte_add++就是指向下一个pte
	for (int ppn = 0; ppn < _num_pages; ppn++) {
		_bit_init(pte_addr, 1);
		pte_addr++;
	}
	// 实际的物理页分布:
	_alloc_start = _align_page(HEAP_START + RESERVERED_NUM * PAGE_SIZE);
	_alloc_end = _alloc_start + (PAGE_SIZE * _num_pages);

	PTE *textptr = (void *)TEXT_START;	 
	printf(".text:   0x%x -> 0x%x, textptr -> %x\n", TEXT_START, TEXT_END, *textptr);
	printf(".rodata: 0x%x -> 0x%x\n", RODATA_START, RODATA_END);
	printf(".data:   0x%x -> 0x%x\n", DATA_START, DATA_END);
	printf(".bss:    0x%x -> 0x%x\n", BSS_START, BSS_END);
	printf(".heap:   0x%x -> 0x%x\n", _alloc_start, _alloc_end);
}


PTE *textpage() {
	PTE *pte_ptr = (PTE *)TEXT_START;
	return pte_ptr;
}


/* npages should be less than or equal to the _num_pages*/
void *page_alloc(int npages)
{

	_num_pages = (HEAP_SIZE / PAGE_SIZE) - RESERVERED_NUM;
	int valid_blocknum = 0; // continous pages => a block
	PTE *pte_ptr = (PTE *)HEAP_START;
	for (int head_ppn = 0; head_ppn <= (_num_pages - npages); head_ppn++) {

		if (_is_free(pte_ptr)) {
			valid_blocknum = 1;
			PTE *following_valid_pte = pte_ptr;
			for (int j = 0; j < npages; j++) {
				following_valid_pte++;
				if (!_is_free(following_valid_pte)) {
					valid_blocknum = 0;
					break;
				}
			}		
			// 整块free就可以直接开始标志物理内存了 
			if (valid_blocknum) { 
				PTE *valid_pte_ptr = pte_ptr; 
				int j;
				for (j = 0; j < npages; j++) {
					_set_flag(valid_pte_ptr, TO_TAKEN_PAGE);
					valid_pte_ptr++;
				}
				valid_pte_ptr--;
				_set_flag(valid_pte_ptr, TO_LAST_PAGE);
				return (void *)(_alloc_start + head_ppn * PAGE_SIZE);
			}

		}

		pte_ptr++;
	}
	return NULL;

}


void page_free(void *head_addr)
{
	if (!head_addr || (uint32_t)head_addr >= _alloc_end) { return; }

	PTE *pte = (PTE *)HEAP_START;
	// pte游标移动物理地址相对_alloc_start的偏移量所代表的页数
	pte += ((uint32_t)head_addr - _alloc_start) / PAGE_SIZE;
	// pte现在一定是指向block的头pte的
	while (!_is_free(pte) && !_is_last(pte)) {
		_clear(pte);
		pte++;
	}
	_clear(pte);
}

void page_test()

{
	void *p = page_alloc(2);
	printf("p = 0x%x\n", p);
	page_free(p);

	void *p2 = page_alloc(7);
	printf("p2 = 0x%x\n", p2);
	page_free(p2);

	void *p3 = page_alloc(4);
	printf("p3 = 0x%x\n", p3);
}
