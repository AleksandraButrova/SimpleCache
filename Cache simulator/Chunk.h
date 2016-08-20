/* Class Chunk describe a part of memore.
A instance include his unique number,
which help calculate included addresses LBA:
range of addresses is from num*chunk_size to (num+1)*chunk_size.*/
class Chunk{
	// here may be data	
public:
	long int num;

	Chunk(long int n) : num(n) {};
};
