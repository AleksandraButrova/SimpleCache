#define lba_size				512								// It is from traces, byte
#define chunk_size				64								// It consists of LBA, Kbyte

#define total_RAM_size			2												// Mbyte
#define prefetch_percent		0.1
#define RAM_size				total_RAM_size * (1 - prefetch_percent)			// Mbyte
#define prefetch_size			total_RAM_size * prefetch_percent				// Mbyte

#define total_RAM_entry_number	total_RAM_size * 1024 * 1024 / chunk_size
#define RAM_entry_num			RAM_size * 1024 / chunk_size					// the number 
#define prefetch_entry_num		prefetch_size * 1024 / chunk_size				// the number 

#define window_size				10									// the number of chunk in one window
#define learning_lim			window_size * 3					// the number of chunks in learning mode (history windows number)

#define min_supp				learning_lim * 0.06									//	Minimal number of windows which supports frequent sequence (single lba or sequence of lba)
#define min_conf				2//window_size * 0.02			/*(?)*/					//	Minimal number of entries item in a window

#define rules_numer				100			/*(?)*/					//	TOP stored rules


