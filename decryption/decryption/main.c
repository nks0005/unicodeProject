#include <stdio.h>
#include <stdlib.h>

int main(void) {
	FILE* F = NULL;

	if (0 != fopen_s(&F, "context.enc", "rb")) {
		printf("Can't find\n");
		exit(1);
	}

	// 파일 크기 구하기
	fseek(F, 0, SEEK_END);
	long size = ftell(F);
	fseek(F, 0, SEEK_SET);

	printf("%d\n", size);


	long index[100][3] = { {0,} };
	long index_offset = 0;
	char checkBuf[4] = { 0, };
	char correctBuf[4] = { 0xA, 0x0, 0x0, 0x0 };
	// 개행 문자 0A 00 00 00,,,
	// 개행 문자.. 0A로 시작됨. 그 뒤의 00 00 00 으로 그 전의 코드 인코딩을 확인할 수 있음

	long oldOffset = 0;
	for (long newOffset = 0; newOffset < (size - 3); newOffset++) {
		fseek(F, newOffset, SEEK_SET);

		for (int i = 0; i < 4; i++)
			fread(&checkBuf[i], 1, 1, F);

		// check 개행문자
		if (
			(checkBuf[0] == correctBuf[0])) {
			// 우선 개행 문자 확인.

			int checkZero = 0;
			for (int i = 1; i < 4; i++) {
				if (checkBuf[i] == 0x00) {
					checkZero++;
				}
			}
			index[index_offset][0] = oldOffset;
			index[index_offset][1] = newOffset + checkZero ;
			index[index_offset++][2] = checkZero;
			oldOffset = newOffset + checkZero ;
		}
	}

	for (long i = 0; i < index_offset; i++) {
		printf("%x ~ %x : %d\n", index[i][0], index[i][1], index[i][2]);
	
		FILE* outFile = NULL;
		char fileName[100] = { 0, };
		char buf[50000] = { 0, };
		sprintf_s(&fileName, 100, "decryption/out_%d.txt", i);
		fopen_s(&outFile, fileName, "wb");


		long length = index[i][1] - index[i][0];
		fseek(F, index[i][0], SEEK_SET);
		fread(buf, 1, length, F);

		fseek(outFile, 0, SEEK_SET);
		fwrite(buf, 1, length, outFile);
		fclose(outFile);

	}

	fclose(F);

}