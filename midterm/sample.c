int add(int a, int b) {
	return a + b;
}

unsigned int sub(unsigned int a, unsigned int b) {
	return a - b;
}

int main() {
  add(1, 0xFFFF'FFFA);     // 1
  add(0x7FFF'FFAC, 0x64);    // 2

  sub(300, 330);             // 3
  sub(0x8000'000A, 16);      // 4
}