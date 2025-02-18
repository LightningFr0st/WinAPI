#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <tuple>

using namespace std;


#define SHARED_MEMORY_NAME L"MySharedMemory"

using namespace std;

struct SharedMemory {
	wchar_t buffer[1024 * 1024 * 10];
	size_t count;
};

void SerializePartialVectorToMemoryN(const vector<tuple<wstring, wstring, wstring>>& vec, SharedMemory* sharedMem, int start, int end) {
	wchar_t* ptr = sharedMem->buffer;
	int ind = 0;
	for (const auto& t : vec) {
		if (ind > end || ind < start) {
			ind++;
			break;
		}
		for (const auto& str : { get<0>(t), get<1>(t), get<2>(t) }) {
			wcscpy(ptr, str.c_str());
			ptr += str.size() + 1;
		}
		break;
	}
}

void SerializeVectorToMemory(const vector<tuple<wstring, wstring, wstring>>& vec, SharedMemory* sharedMem) {
	wchar_t* ptr = sharedMem->buffer;
	for (const auto& t : vec) {
		for (const auto& str : { get<0>(t), get<1>(t), get<2>(t) }) {
			wcscpy(ptr, str.c_str());
			ptr += str.size() + 1;
		}
	}
	sharedMem->count = vec.size();
}

vector<tuple<wstring, wstring, wstring>> DeserializeVectorFromMemory(const SharedMemory* sharedMem) {
	vector<tuple<wstring, wstring, wstring>> vec(sharedMem->count);
	const wchar_t* ptr = sharedMem->buffer;

	for (auto& t : vec) {
		wstring first = ptr;
		ptr += first.size() + 1;
		wstring second = ptr;
		ptr += second.size() + 1;
		wstring third = ptr;
		ptr += third.size() + 1;
		t = make_tuple(first, second, third);
	}

	return vec;
}

void Merge(vector<tuple<wstring, wstring, wstring>>& vec, int low, int mid, int high, int sortIndex) {
	vector<tuple<wstring, wstring, wstring>> left(vec.begin() + low, vec.begin() + mid + 1);
	vector<tuple<wstring, wstring, wstring>> right(vec.begin() + mid + 1, vec.begin() + high + 1);

	int i = 0, j = 0, k = low;
	while (i < left.size() && j < right.size()) {
		const auto& l = left[i];
		const auto& r = right[j];

		if (sortIndex == 0 ? get<0>(l) <= get<0>(r)
			: sortIndex == 1 ? get<1>(l) <= get<1>(r)
			: get<2>(l) <= get<2>(r)) {
			vec[k++] = l;
			i++;
		}
		else {
			vec[k++] = r;
			j++;
		}
	}

	while (i < left.size()) vec[k++] = left[i++];
	while (j < right.size()) vec[k++] = right[j++];
}

void MergeSort(vector<tuple<wstring, wstring, wstring>>& vec, int low, int high, int sortIndex) {
	if (low < high) {
		int mid = low + (high - low) / 2;
		MergeSort(vec, low, mid, sortIndex);
		MergeSort(vec, mid + 1, high, sortIndex);
		Merge(vec, low, mid, high, sortIndex);
	}
}

int wmain(int argc, wchar_t* argv[]) {

	if (argc != 4) {
		wcerr << L"Invalid arguments. Expected: start_index end_index sort_index\n";
		return 1;
	}

	int start = _wtoi(argv[1]);
	int end = _wtoi(argv[2]);
	int sortIndex = _wtoi(argv[3]);

	HANDLE hMapFile = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, SHARED_MEMORY_NAME);
	if (hMapFile == nullptr) {
		wcerr << L"Failed to open shared memory: " << GetLastError() << endl;
		return 1;
	}

	SharedMemory* sharedMem = (SharedMemory*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedMemory));
	if (sharedMem == nullptr) {
		wcerr << L"Failed to map shared memory: " << GetLastError() << endl;
		CloseHandle(hMapFile);
		return 1;
	}

	vector<tuple<wstring, wstring, wstring>> vec = DeserializeVectorFromMemory(sharedMem);

	MergeSort(vec, start, end, sortIndex);

	SerializeVectorToMemory(vec, sharedMem);

	UnmapViewOfFile(sharedMem);
	CloseHandle(hMapFile);

	return 0;
}