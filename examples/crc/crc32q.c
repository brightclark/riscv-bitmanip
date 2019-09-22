#include <rvintrin.h>
#include <assert.h>
#include <stdio.h>

#define P  0x1814141ABUL
#define P0  0x814141AB
#define mu 0x1FEFF7F62UL
#define mu1 0xFF7FBFB1
#define k1  0xA1FA6BEC
#define k2  0x9BE9878F
#define k3  0xB1EFC5F6

static inline int64_t rdinstret() { int64_t rd; asm volatile ("rdinstret %0" : "=r"(rd)); return rd; }

uint32_t crc32q_table[256] = {
	0x00000000, 0x814141ab, 0x83c3c2fd, 0x02828356, 0x86c6c451, 0x078785fa, 0x050506ac, 0x84444707,
	0x8cccc909, 0x0d8d88a2, 0x0f0f0bf4, 0x8e4e4a5f, 0x0a0a0d58, 0x8b4b4cf3, 0x89c9cfa5, 0x08888e0e,
	0x98d8d3b9, 0x19999212, 0x1b1b1144, 0x9a5a50ef, 0x1e1e17e8, 0x9f5f5643, 0x9dddd515, 0x1c9c94be,
	0x14141ab0, 0x95555b1b, 0x97d7d84d, 0x169699e6, 0x92d2dee1, 0x13939f4a, 0x11111c1c, 0x90505db7,
	0xb0f0e6d9, 0x31b1a772, 0x33332424, 0xb272658f, 0x36362288, 0xb7776323, 0xb5f5e075, 0x34b4a1de,
	0x3c3c2fd0, 0xbd7d6e7b, 0xbfffed2d, 0x3ebeac86, 0xbafaeb81, 0x3bbbaa2a, 0x3939297c, 0xb87868d7,
	0x28283560, 0xa96974cb, 0xabebf79d, 0x2aaab636, 0xaeeef131, 0x2fafb09a, 0x2d2d33cc, 0xac6c7267,
	0xa4e4fc69, 0x25a5bdc2, 0x27273e94, 0xa6667f3f, 0x22223838, 0xa3637993, 0xa1e1fac5, 0x20a0bb6e,
	0xe0a08c19, 0x61e1cdb2, 0x63634ee4, 0xe2220f4f, 0x66664848, 0xe72709e3, 0xe5a58ab5, 0x64e4cb1e,
	0x6c6c4510, 0xed2d04bb, 0xefaf87ed, 0x6eeec646, 0xeaaa8141, 0x6bebc0ea, 0x696943bc, 0xe8280217,
	0x78785fa0, 0xf9391e0b, 0xfbbb9d5d, 0x7afadcf6, 0xfebe9bf1, 0x7fffda5a, 0x7d7d590c, 0xfc3c18a7,
	0xf4b496a9, 0x75f5d702, 0x77775454, 0xf63615ff, 0x727252f8, 0xf3331353, 0xf1b19005, 0x70f0d1ae,
	0x50506ac0, 0xd1112b6b, 0xd393a83d, 0x52d2e996, 0xd696ae91, 0x57d7ef3a, 0x55556c6c, 0xd4142dc7,
	0xdc9ca3c9, 0x5ddde262, 0x5f5f6134, 0xde1e209f, 0x5a5a6798, 0xdb1b2633, 0xd999a565, 0x58d8e4ce,
	0xc888b979, 0x49c9f8d2, 0x4b4b7b84, 0xca0a3a2f, 0x4e4e7d28, 0xcf0f3c83, 0xcd8dbfd5, 0x4cccfe7e,
	0x44447070, 0xc50531db, 0xc787b28d, 0x46c6f326, 0xc282b421, 0x43c3f58a, 0x414176dc, 0xc0003777,
	0x40005999, 0xc1411832, 0xc3c39b64, 0x4282dacf, 0xc6c69dc8, 0x4787dc63, 0x45055f35, 0xc4441e9e,
	0xcccc9090, 0x4d8dd13b, 0x4f0f526d, 0xce4e13c6, 0x4a0a54c1, 0xcb4b156a, 0xc9c9963c, 0x4888d797,
	0xd8d88a20, 0x5999cb8b, 0x5b1b48dd, 0xda5a0976, 0x5e1e4e71, 0xdf5f0fda, 0xdddd8c8c, 0x5c9ccd27,
	0x54144329, 0xd5550282, 0xd7d781d4, 0x5696c07f, 0xd2d28778, 0x5393c6d3, 0x51114585, 0xd050042e,
	0xf0f0bf40, 0x71b1feeb, 0x73337dbd, 0xf2723c16, 0x76367b11, 0xf7773aba, 0xf5f5b9ec, 0x74b4f847,
	0x7c3c7649, 0xfd7d37e2, 0xffffb4b4, 0x7ebef51f, 0xfafab218, 0x7bbbf3b3, 0x793970e5, 0xf878314e,
	0x68286cf9, 0xe9692d52, 0xebebae04, 0x6aaaefaf, 0xeeeea8a8, 0x6fafe903, 0x6d2d6a55, 0xec6c2bfe,
	0xe4e4a5f0, 0x65a5e45b, 0x6727670d, 0xe66626a6, 0x622261a1, 0xe363200a, 0xe1e1a35c, 0x60a0e2f7,
	0xa0a0d580, 0x21e1942b, 0x2363177d, 0xa22256d6, 0x266611d1, 0xa727507a, 0xa5a5d32c, 0x24e49287,
	0x2c6c1c89, 0xad2d5d22, 0xafafde74, 0x2eee9fdf, 0xaaaad8d8, 0x2beb9973, 0x29691a25, 0xa8285b8e,
	0x38780639, 0xb9394792, 0xbbbbc4c4, 0x3afa856f, 0xbebec268, 0x3fff83c3, 0x3d7d0095, 0xbc3c413e,
	0xb4b4cf30, 0x35f58e9b, 0x37770dcd, 0xb6364c66, 0x32720b61, 0xb3334aca, 0xb1b1c99c, 0x30f08837,
	0x10503359, 0x911172f2, 0x9393f1a4, 0x12d2b00f, 0x9696f708, 0x17d7b6a3, 0x155535f5, 0x9414745e,
	0x9c9cfa50, 0x1dddbbfb, 0x1f5f38ad, 0x9e1e7906, 0x1a5a3e01, 0x9b1b7faa, 0x9999fcfc, 0x18d8bd57,
	0x8888e0e0, 0x09c9a14b, 0x0b4b221d, 0x8a0a63b6, 0x0e4e24b1, 0x8f0f651a, 0x8d8de64c, 0x0ccca7e7,
	0x044429e9, 0x85056842, 0x8787eb14, 0x06c6aabf, 0x8282edb8, 0x03c3ac13, 0x01412f45, 0x80006eee
};

uint8_t testdata[1024] = {
	// for i in {0..1023}; do echo $(( RANDOM % 255)), ; done | fmt
	246, 220, 231, 190, 227, 151, 204, 166, 41, 242, 111, 97, 44, 87, 184,
	121, 85, 240, 77, 11, 73, 85, 184, 192, 134, 33, 126, 194, 244, 152, 37,
	30, 109, 110, 247, 105, 119, 124, 207, 45, 228, 242, 227, 47, 158, 197,
	126, 160, 45, 94, 223, 55, 30, 11, 9, 47, 142, 229, 223, 169, 13, 232,
	58, 233, 53, 102, 249, 181, 37, 55, 182, 60, 55, 19, 212, 176, 193, 13,
	197, 224, 155, 172, 14, 149, 107, 97, 123, 19, 159, 110, 22, 244, 23,
	55, 149, 84, 103, 96, 139, 30, 5, 25, 231, 12, 151, 78, 123, 182, 191,
	207, 250, 34, 98, 214, 207, 7, 210, 8, 185, 7, 65, 57, 106, 162, 192,
	226, 56, 225, 33, 49, 25, 139, 225, 20, 107, 69, 52, 82, 54, 94, 58,
	160, 106, 206, 208, 66, 83, 144, 206, 73, 3, 165, 158, 152, 221, 237,
	205, 24, 68, 185, 56, 9, 253, 204, 172, 35, 249, 29, 202, 183, 136, 11,
	194, 115, 189, 238, 130, 205, 99, 18, 247, 162, 111, 153, 84, 18, 59,
	92, 114, 205, 194, 85, 74, 219, 246, 81, 225, 142, 98, 136, 205, 45, 234,
	253, 51, 103, 59, 217, 238, 139, 52, 72, 237, 146, 218, 99, 126, 37, 54,
	45, 131, 237, 131, 211, 5, 65, 97, 168, 43, 101, 154, 76, 180, 145, 150,
	161, 185, 188, 27, 199, 21, 208, 163, 214, 166, 23, 19, 131, 146, 47,
	250, 38, 69, 189, 175, 130, 222, 81, 186, 59, 86, 31, 199, 54, 12, 18,
	96, 170, 205, 130, 192, 186, 116, 61, 112, 135, 223, 120, 61, 23, 35,
	190, 131, 197, 163, 139, 222, 21, 202, 73, 214, 164, 129, 35, 77, 199,
	178, 3, 203, 228, 54, 132, 30, 199, 7, 253, 5, 244, 64, 93, 213, 199,
	47, 115, 99, 117, 143, 247, 204, 182, 80, 32, 151, 76, 72, 120, 149, 221,
	189, 72, 245, 18, 28, 217, 59, 225, 154, 88, 178, 75, 156, 87, 93, 160,
	92, 80, 83, 32, 107, 108, 9, 155, 203, 139, 22, 214, 111, 16, 209, 100,
	123, 56, 115, 178, 38, 0, 77, 96, 248, 106, 238, 2, 73, 202, 253, 28,
	74, 0, 207, 82, 12, 61, 212, 226, 161, 128, 12, 94, 91, 42, 160, 240,
	160, 199, 246, 195, 120, 142, 42, 18, 27, 202, 231, 226, 170, 180, 161,
	244, 1, 105, 245, 166, 213, 165, 140, 236, 156, 193, 107, 166, 147, 200,
	135, 241, 80, 22, 168, 74, 208, 248, 199, 69, 139, 160, 236, 242, 193,
	114, 224, 133, 237, 125, 121, 96, 44, 13, 131, 38, 166, 106, 70, 116,
	121, 52, 204, 135, 224, 68, 117, 69, 183, 107, 71, 237, 13, 101, 193, 9,
	146, 99, 162, 220, 172, 230, 151, 186, 204, 103, 170, 197, 124, 51, 82,
	64, 227, 63, 207, 185, 61, 71, 82, 53, 45, 2, 199, 46, 253, 138, 161,
	75, 90, 145, 204, 48, 52, 144, 225, 24, 22, 15, 97, 17, 40, 238, 172,
	251, 82, 95, 42, 149, 172, 190, 228, 196, 177, 250, 135, 91, 10, 239,
	152, 154, 179, 55, 34, 182, 171, 75, 46, 71, 91, 34, 128, 135, 200, 15,
	249, 202, 89, 208, 107, 116, 64, 84, 6, 150, 27, 194, 169, 237, 109, 162,
	54, 248, 167, 94, 95, 202, 62, 235, 42, 161, 245, 58, 134, 224, 47, 78,
	93, 1, 163, 132, 209, 252, 222, 195, 205, 162, 164, 95, 132, 194, 38,
	207, 52, 125, 10, 111, 13, 125, 182, 102, 96, 246, 243, 251, 198, 166,
	0, 5, 38, 116, 235, 214, 28, 1, 29, 245, 114, 198, 237, 1, 92, 68, 125,
	192, 33, 69, 231, 156, 0, 39, 231, 191, 103, 13, 104, 176, 47, 58, 92,
	253, 141, 128, 210, 117, 27, 246, 2, 95, 179, 234, 95, 153, 74, 142,
	127, 94, 234, 161, 159, 83, 41, 222, 222, 178, 194, 61, 17, 55, 209,
	1, 39, 199, 70, 196, 11, 4, 15, 227, 205, 19, 174, 100, 32, 43, 147,
	75, 27, 227, 190, 84, 160, 37, 45, 5, 36, 150, 42, 8, 0, 190, 67, 155,
	245, 50, 202, 250, 158, 205, 38, 103, 22, 171, 231, 90, 52, 41, 212,
	54, 145, 94, 122, 181, 121, 163, 183, 153, 142, 9, 36, 111, 160, 240,
	143, 136, 175, 61, 222, 190, 40, 179, 227, 173, 14, 212, 125, 97, 153,
	99, 71, 42, 19, 187, 211, 4, 154, 222, 199, 18, 195, 197, 77, 219, 105,
	131, 237, 242, 103, 32, 178, 114, 66, 191, 213, 43, 52, 237, 175, 80,
	195, 236, 147, 122, 135, 187, 32, 172, 89, 55, 126, 190, 22, 55, 252,
	8, 221, 73, 60, 70, 81, 159, 17, 224, 224, 85, 237, 135, 67, 92, 213,
	52, 193, 90, 42, 73, 205, 7, 14, 154, 100, 226, 164, 218, 128, 127, 46,
	246, 136, 7, 224, 108, 29, 35, 56, 191, 210, 211, 5, 70, 96, 233, 166,
	213, 30, 128, 144, 29, 37, 192, 197, 196, 118, 241, 161, 126, 182, 137,
	150, 118, 127, 113, 16, 138, 204, 109, 104, 110, 190, 56, 63, 216, 251,
	133, 96, 183, 62, 73, 193, 165, 109, 1, 61, 111, 108, 228, 159, 207, 32,
	12, 107, 190, 109, 165, 201, 102, 67, 44, 96, 156, 54, 206, 85, 132, 15,
	227, 211, 234, 97, 252, 146, 115, 191, 143, 196, 1, 143, 238, 33, 207,
	70, 240, 132, 23, 1, 10, 173, 179, 22, 166, 208, 103, 124, 17, 201, 69,
	94, 90, 137, 87, 144, 152, 141, 86, 58, 30, 80, 42, 181, 155, 114, 125,
	205, 216, 184, 135, 117, 134, 142, 160, 161, 245, 39, 13, 23, 190, 211,
	155, 14, 132, 254, 6, 109, 147, 211, 217, 117, 94, 185, 159, 234, 155,
	179, 27, 127, 62, 105, 199, 113, 136, 148, 108, 99, 7, 73, 236, 33, 33,
	156, 152, 2, 34, 14, 24, 208, 47, 217, 17, 248, 207, 233, 33, 202, 71,
	12, 253, 154, 121, 43, 72, 106, 96, 13, 203
};

uint32_t crc32q_lookup(const uint8_t *data, int len)
{
	uint32_t crc = 0;
	const uint8_t *end = data + len;
	while (data != end) {
		int i = (crc >> 24) ^ *(data++);
		crc = (crc << 8) ^ crc32q_table[i];
	}
	return crc;
}

uint32_t crc32q_barrett(const uint32_t *data, int len)
{
	uint32_t crc = 0;
	const uint32_t *end = data + len;
	while (data != end) {
		crc ^= _rv32_rev8(*(data++));
		crc = _rv32_clmulr(crc, mu1);
		crc = _rv32_clmul(crc, P0);
	}
	return crc;
}

uint32_t crc32q_fold(const uint64_t *data, int len)
{
	const uint64_t *end = data + len;
	uint64_t a0, a1, a2, t1, t2;

	assert(len >= 2);
	a0 = _rv64_rev8(*(data++));
	a1 = _rv64_rev8(*(data++));

	// Main loop: Reduce to 2x 64 bits
	while (data != end) {
		a2 = _rv64_rev8(*(data++));
		t1 = _rv64_clmulh(a0, k1);
		t2 = _rv64_clmul(a0, k1);
		a0 = a1 ^ t1;
		a1 = a2 ^ t2;
	}

	// Reduce to 64 bit, add 32 bit zero padding
	t1 = _rv64_clmulh(a0, k2);
	t2 = _rv64_clmul(a0, k2);
	a0 = (a1 >> 32) ^ t1;
	a1 = (a1 << 32) ^ t2;
	t2 = _rv64_clmul(a0, k3);
	a1 = a1 ^ t2;

	// Barrett Reduction
	t1 = _rv64_clmul(a1 >> 32, mu);
	t2 = _rv64_clmul(t1 >> 32, P);
	a0 = a1 ^ t2;

	return a0;
}

int main()
{
	uint32_t crc, ref;
	int t;

	crc = crc32q_lookup((const uint8_t*)"123456789", 9);
	printf("check 0x%08x\n", crc);
	assert(crc == 0x3010bf7f);

	printf("\n");

	t = rdinstret();
	crc = crc32q_lookup(testdata, 1024);
	t = rdinstret() - t;
	printf("crc32q_lookup  0x%08x %5d\n", crc, t);
	ref = crc;

	t = rdinstret();
	crc = crc32q_barrett((uint32_t*)testdata, 1024 / 4);
	t = rdinstret() - t;
	printf("crc32q_barrett 0x%08x %5d\n", crc, t);
	assert(ref == crc);

	t = rdinstret();
	crc = crc32q_fold((uint64_t*)testdata, 1024 / 8);
	t = rdinstret() - t;
	printf("crc32q_fold    0x%08x %5d\n", crc, t);
	assert(ref == crc);

	printf("\n");

	return 0;
}
