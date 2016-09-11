#define lba_size				512								// It is from traces, byte
#define chunk_size				1024//512								// It consists of LBA, Kbyte

#define total_RAM_size			10000												// Kbyte
#define prefetch_percent		0.0005

#define RAM_size				total_RAM_size * (1 - prefetch_percent)			// Kbyte
#define prefetch_size			total_RAM_size * prefetch_percent				// Kbyte

#define total_RAM_entry_number	total_RAM_size * 1024 / chunk_size
#define RAM_entry_num			RAM_size * 1024 / chunk_size					// the number 
#define prefetch_entry_num		prefetch_size * 1024 / chunk_size				// the number 

#define window_size				5								// the number of chunk in one window
#define learning_lim			window_size * 3					// the number of chunks in learning mode (history windows number)

#define min_supp				3							//	Minimal number of windows which supports frequent sequence (single lba or sequence of lba)


