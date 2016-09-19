#define prefetch_percent		0.05
#define total_RAM_entry_num		1e4
#define RAM_entry_num			(1 - prefetch_percent) * total_RAM_entry_num
#define prefetch_entry_num		prefetch_percent * total_RAM_entry_num

#define window_size				5			// the number of chunk in one window
#define min_supp				3				//	Minimal number of windows which supports frequent sequence (single lba or sequence of lba)


