/*
################################
The project was made by JaKeN525
unDAT for
Beetle Crazy Cup/Kaefer Total/
Beetle Buggin'/Radical Drive
################################

 ------------------------------
|   ###    ###    ###    ###   |
|  #   #  #   #  #   #  #   #  |
|     #   #   #     #      #   |
|    #    #   #    #    #   #  |
|  #####   ###   #####   ###   |
 ------------------------------
*/

#include <iostream>
#include <fstream>
#include <shlwapi.h>
#include <filesystem>
#include <conio.h>
#include "obj2inf.h"
#include "mk64n.h"
#include "Screen.h"
#include "originalLocale.h"

namespace fs = std::filesystem;

int screen = 1;
int backUp = 1;

std::vector<char> namesID = { 0, 0, 0, 0 };

const std::vector<std::string> names = { "artwork", "3d", "wave", "menus" };
std::string currentPath = fs::current_path().string();

std::string openfilename(HWND owner = NULL, uint32_t flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY)
{
	std::string filename(MAX_PATH, '\0');
	OPENFILENAME ofn = { };

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = owner;
	ofn.lpstrFilter = "TXT Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = &filename[0];
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Select a File";
	ofn.Flags = flags;

	if (!GetOpenFileName(&ofn))
		return "";

	return filename;
}

void DisplayInfo(std::string info, bool isInfo)
{
	system("cls");

	std::cout << std::endl << " ";

	if (isInfo)
		std::cout << "[INFO] ";
	std::cout << info << std::endl;
}

std::string ReadStringWithout(char*& f, size_t& pos, int size)
{
	std::string result = "";
	bool isWriting = true;
	
	for (int i = 0; i < size; i++) 
	{
		if (f[pos] != 0 && isWriting)
			result += f[pos];
		else
			isWriting = false;
		pos += 1;
	}

	return result;
}

std::string ReadString(char*& f, size_t& pos, int size)
{
	std::string result = "";

	for (int i = 0; i < size; i++)
	{
		result += f[pos];
		pos += 1;
	}

	return result;
}

std::string WriteString(int size, std::string str) 
{
	size -= str.length();

	std::string endLine = "";
	endLine += str;

	for (int i = 0; i < size; i++)
		endLine += char(uint8_t(0));

	return endLine;
}

std::string WriteLong(int num)
{
	std::string Hex = "";
	std::stringstream s;
	s << std::hex << num;

	int zeroes = 8 - size(s.str());
	for (int i = 0; i < zeroes; i++)
		Hex += "0";
	Hex += s.str();

	int arr[] = { 0, 0, 0, 0 };
	int c = 3;
	for (int i = 0; i < Hex.length() - 1; i += 2)
	{
		std::stringstream h;
		h << Hex[i] << Hex[i + 1];
		h >> std::hex >> arr[c];
		c--;
	}

	Hex = "";
	for (int i = 0; i < 4; i++)
		Hex += char(uint8_t(arr[i]));

	return Hex;
}

unsigned long readlong(char*& f, size_t& pos) 
{
	unsigned long result = 
	{
		(unsigned int)((uint8_t)f[pos] * 0x00000001) + \
		(unsigned int)((uint8_t)f[pos + 1] * 0x00000100) + \
		(unsigned int)((uint8_t)f[pos + 2] * 0x00010000) + \
		(unsigned int)((uint8_t)f[pos + 3] * 0x01000000)
	};
	pos += 4;
	return result;
}

bool OpenFile(char*& f, size_t& fsize, std::string filename)
{
	HANDLE hFile = CreateFile(
		filename.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hFile == INVALID_HANDLE_VALUE) 
	{
		DisplayInfo("Failed to Open File ", true);
		return false;
	}

	fsize = GetFileSize(hFile, NULL);
	if (fsize == 0) 
	{
		DisplayInfo("Failed to read file. File is Empty?", true);
		return false;
	}

	f = new char[fsize];
	unsigned long dwBytesRead = 0;

	if (ReadFile(hFile, f, fsize, &dwBytesRead, NULL) == FALSE || dwBytesRead != fsize) 
	{
		DisplayInfo("Failed to copy file into memory", true);
		fsize = 0;
		delete[] f;
		f = NULL;
		CloseHandle(hFile);
		return false;
	}
	CloseHandle(hFile);
}

void SaveOptions()
{
	std::ofstream file(currentPath + "\\settings.ini", std::ios::binary);

	file << WriteLong(screen)
		 << WriteLong(backUp);

	for (int i = 0; i < names.size(); i++)
		file << WriteLong(namesID[i]);

	file.close();
}

void LoadOptions()
{
	if (!fs::exists(currentPath + "\\settings.ini"))
	{
		std::ofstream file(currentPath + "\\settings.ini", std::ios::binary);

		file << WriteLong(0)
			 << WriteLong(0)
			 << WriteLong(1)
			 << WriteLong(1)
			 << WriteLong(1)
			 << WriteLong(1);

		file.close();
	}
	else
	{
		char* f = NULL;
		size_t fsize = 0;
		size_t ptr = 0;

		if (OpenFile(f, fsize, currentPath + "\\settings.ini"))
		{
			screen = readlong(f, ptr);
			backUp = readlong(f, ptr);

			for (int i = 0; i < names.size(); i++)
				namesID[i] = readlong(f, ptr);
		}

		if (f != NULL)
		{
			delete[] f;
			f = NULL;
		}
	}
}

struct DAT 
{
	uint32_t num_folders = 0;
	uint32_t num_files = 0;

	uint32_t* folderID;
	uint32_t* fileFolderID;
	uint32_t* fileFolderIDTemp;
	size_t* fileOffset;
	uint32_t* fileSize;
	uint32_t* fileTSTAMPfst;
	uint32_t* fileTSTAMPsnd;

	std::string* folderName;
	std::string* fileName;
	std::vector<std::string> filesPath;

	DAT() 
	{
		folderID = NULL;
		folderName = NULL;
		fileName = NULL;
		fileOffset = NULL;
		fileSize = NULL;
		fileTSTAMPfst = NULL;
		fileTSTAMPsnd = NULL;
		fileFolderID = NULL;
		fileFolderIDTemp = NULL;
	}
	~DAT() 
	{
		if (folderID = NULL) delete[] folderID;
		if (folderName = NULL) delete[] folderName;
		if (fileName = NULL) delete[] fileName;
		if (fileOffset = NULL) delete[] fileOffset;
		if (fileSize = NULL) delete[] fileSize;
		if (fileTSTAMPfst = NULL) delete[] fileTSTAMPfst;
		if (fileTSTAMPsnd = NULL) delete[] fileTSTAMPsnd;
		if (fileFolderID = NULL) delete[] fileFolderID;
		if (fileFolderIDTemp = NULL) delete[] fileFolderIDTemp;
	}

	bool ReadAndWriteFolders(char*& f, size_t& pos, size_t& fsize, std::string n) 
	{
		if (!fs::exists(currentPath + "\\" + n))
			fs::create_directory(currentPath + "\\" + n);

		bool sucess = false;
		num_folders = readlong(f, pos);
		num_files = readlong(f, pos);

		folderID = new uint32_t[num_folders];
		folderName = new std::string[num_folders];

		fileName = new std::string[num_files];
		fileOffset = new size_t[num_files];
		fileSize = new uint32_t[num_files];
		fileTSTAMPfst = new uint32_t[num_files];
		fileTSTAMPsnd = new uint32_t[num_files];
		fileFolderID = new uint32_t[num_files];
		fileFolderIDTemp = new uint32_t[num_files];

		// folders
		for (int i = 0; i < num_folders; i++) 
		{
			folderID[i] = readlong(f, pos);
			folderName[i] = ReadStringWithout(f, pos, 128);
		}

		// files
		for (int i = 0; i < num_files; i++)
		{
			fileFolderID[i] = readlong(f, pos);
			fileFolderIDTemp[i] = fileFolderID[i];
			fileName[i] = ReadStringWithout(f, pos, 128);
			fileOffset[i] = readlong(f, pos);
			fileSize[i] = readlong(f, pos);

			pos += 4; // skip zeroes

			fileTSTAMPfst[i] = readlong(f, pos);
			fileTSTAMPsnd[i] = readlong(f, pos);

			std::vector<std::string> folders;
			while (fileFolderID[i] != 0)
			{
				fileFolderID[i] -= 1;

				folders.push_back(folderName[fileFolderID[i]]);
				fileFolderID[i] = folderID[fileFolderID[i]];
			}

			std::string path = currentPath + "\\" + n + "\\";
			for (int j = folders.size() - 1; j >= 0; j--)
			{
				path += folders[j] + "\\";

				if (!fs::exists(path))
					fs::create_directory(path);
			}

			filesPath.push_back(path);
		}

		return sucess;
	}

	void WriteFiles(char*& f, size_t& pos, size_t& fsize)
	{
		for (int i = 0; i < num_files; i++)
		{
			std::string filepath = filesPath[i] + fileName[i];

			std::ofstream file(filepath, std::ios_base::binary);
			file << ReadString(f, fileOffset[i], fileSize[i]);
			file.close();
		}
	}

	void CreateArchive(std::string n, bool cBackup, bool cStartScreen)
	{
		DisplayInfo("[" + n + "] Creating a temp file.", false);

		std::ofstream tempWrite(currentPath + "\\" + n + ".temp", std::ios::binary);
		size_t archiveIntro = (num_folders * 4 + num_folders * 128) + (num_files * 8 + num_files * 128 + num_files * 8) + 8;
		size_t temp = 0;

		for (int i = 0; i < num_files; i++)
		{
			if (fs::exists(filesPath[i] + fileName[i]))
			{
				char* f = NULL;
				size_t fsize = 0;
				size_t ptr = 0;

				if (OpenFile(f, fsize, filesPath[i] + fileName[i]))
				{
					fileOffset[i] = archiveIntro + temp;

					if (fileName[i] == "CARGANDO.BMP" && cStartScreen)
					{
						fileSize[i] = std::size(tex::startScreen);
						temp += std::size(tex::startScreen) + 4;

						for (int j = 0; j < std::size(tex::startScreen); j++)
							tempWrite << char(tex::startScreen[j]);

						tempWrite << WriteLong(0);
					}
					else 
					{
						fileSize[i] = fsize;

						temp += fsize + 4;
						tempWrite << ReadString(f, ptr, fsize) << WriteLong(0);
					}
				}

				if (f != NULL)
				{
					delete[] f;
					f = NULL;
				}
			}
			else 
			{
				DisplayInfo("[" + n + "] The file was not found. Try unpacking the archive first.", false);

				tempWrite.close();
				fs::remove(currentPath + "\\" + n + ".temp");

				return;
			}
		}

		tempWrite.close();

		DisplayInfo("[" + n + "] Creating a new archive.", false);

		if (cBackup)
			fs::rename(currentPath + "\\" + n + ".dat", n + "old.dat");
		else
			fs::remove(currentPath + "\\" + n + ".dat");

		std::ofstream archive(currentPath + "\\" + n + ".dat", std::ios::binary);

		archive << WriteLong(num_folders) << WriteLong(num_files);
		for (int i = 0; i < num_folders; i++)
			archive << WriteLong(folderID[i]) << WriteString(128, folderName[i]);

		for (int i = 0; i < num_files; i++)
			archive << WriteLong(fileFolderIDTemp[i])
					<< WriteString(128, fileName[i])
					<< WriteLong(fileOffset[i])
					<< WriteLong(fileSize[i])
					<< WriteLong(0)
					<< WriteLong(fileTSTAMPfst[i])
					<< WriteLong(fileTSTAMPsnd[i]);

		if (fs::exists(currentPath + "\\" + n + ".temp"))
		{
			char* f = NULL;
			size_t fsize = 0;
			size_t ptr = 0;

			if (OpenFile(f, fsize, n + ".temp"))
				archive << ReadString(f, ptr, fsize);

			if (f != NULL)
			{
				delete[] f;
				f = NULL;
			}
		}
		else 
		{
			DisplayInfo("[" + n + "] Temp file was not found. Try not to delete it and start over.", false);

			archive.close();
			return;
		}

		archive.close();
		fs::remove(currentPath + "\\" + n + ".temp");

		DisplayInfo("The archive was successfully created.", true);
	}
};

void Extract() 
{
	for (int i = 0; i < names.size(); i++)
	{
		if (namesID[i] == 1)
		{
			char* f = NULL;
			size_t fsize = 0;

			DisplayInfo("Trying to open " + names[i] + ".dat", true);

			if (OpenFile(f, fsize, currentPath + "\\" + names[i] + ".dat"))
			{
				DAT dat;
				size_t ptr = 0;

				DisplayInfo("[" + names[i] + "] Reading the archive and creating directories.", false);
				dat.ReadAndWriteFolders(f, ptr, fsize, names[i]);

				DisplayInfo("[" + names[i] + "] Extracting files.", false);
				dat.WriteFiles(f, ptr, fsize);
			}

			if (f != NULL)
			{
				delete[] f;
				f = NULL;
			}
		}
	}

	DisplayInfo("Unpacking is over.", true);
}

void PackBack() 
{
	bool createBackup = true;
	bool changeStartScreen = true;

	if (backUp == 0) 
	{
		DisplayInfo("Do you need to create backup files?\n Press Y or N.\n", true);

		backUp = _getch();

		if (backUp > 90)
			backUp -= 32;

		if (backUp == 89 || backUp == 78)
			SaveOptions();
		else
		{
			DisplayInfo("Data entered incorrectly.", true);
			return;
		}
	}

	switch (backUp)
	{
	case 89:
		createBackup = true;
		break;

	case 78:
		createBackup = false;
		break;

	default:
		DisplayInfo("Data entered incorrectly.", true);
		break;
	}


	if (screen == 0)
	{
		DisplayInfo("Do you need to change start screen?\n Press Y or N.\n", true);

		screen = _getch();

		if (screen > 90)
			screen -= 32;

		if (screen == 89 || screen == 78)
			SaveOptions();
		else
		{
			DisplayInfo("Data entered incorrectly.", true);
			return;
		}
	}

	switch (screen)
	{
	case 89:
		changeStartScreen = true;
		break;

	case 78:
		changeStartScreen = false;
		break;

	default:
		DisplayInfo("Data entered incorrectly.", true);
		break;
	}

	for (int i = 0; i < names.size(); i++)
	{
		if (namesID[i] == 1) 
		{
			char* f = NULL;
			size_t fsize = 0;

			if (fs::exists(currentPath + "\\" + names[i]))
			{
				DisplayInfo("Trying to open " + names[i] + ".dat", true);

				if (OpenFile(f, fsize, currentPath + "\\" + names[i] + ".dat"))
				{
					DAT dat;
					size_t ptr = 0;

					DisplayInfo("[" + names[i] + "] Reading the archive and creating directories.", false);
					dat.ReadAndWriteFolders(f, ptr, fsize, names[i]);

					DisplayInfo("[" + names[i] + "] Writing files.", false);
					dat.CreateArchive(names[i], createBackup, changeStartScreen);
				}

				if (f != NULL)
				{
					delete[] f;
					f = NULL;
				}
			}
			else
				DisplayInfo("The " + names[i] + " folder was not found.", true);
		}
	}
}

void Obj2Inf() 
{
	DisplayInfo("Trying to open obj file.", true);

	otoi::obj2inf obj(currentPath);
	obj.Convert();
}

void Inf2Obj()
{
	DisplayInfo("The program was made by mariokart64n (coreynguyen) from XeNTaX\n https://forum.xentax.com/memberlist.php?mode=viewprofile&u=531\n https://github.com/coreynguyen/beetle_crazy_cup", true);

	itoo::inf2obj inf(currentPath);
	inf.Convert();
}

void Help() 
{
	system("cls");
	std::cout << std::endl;
	srand(time(NULL));

	std::string info = " Beetle Crazy Cup Tool v1.3 by JaKeN525Help:";

	for (int i = 0; i < info.length(); i++)
		if (info[i] != 72) 
		{
			Sleep(50 + rand() % 401);
			std::cout << info[i];
		}
		else 
		{
			Sleep(1000 + rand() % 1501);
			std::cout << std::endl << " " << info[i];
		}
	Sleep(1000);

	std::cout << std::endl << " - dat unpacker. Unpack all files in the archives: 3d, artwork, wave and menus." << std::endl;
	Sleep(100);
	std::cout << std::endl << " - dat packer. Creates a new archive based on the received data. If one\n of the folders does not exist, then the archive will not be\n created. It is advisable to delete unnecessary files that\n should not be in the archive." << std::endl;
	Sleep(100);
	std::cout << std::endl << " - obj2inf. Converter of 3d models from wavefront obj files to inf, ver,\n tri and nor. It is advisable to use models of no more than\n 2k polygons. Textures should have a color depth of 8 bits, 256x256\n pixels with a resolution of 72 dpi in bmp format. The convertible\n model must be adjusted in size." << std::endl;
	Sleep(100);
	std::cout << std::endl << " - inf2obj. Converter of 3d models from inf, ver, tri files to wavefront obj.\n Made by mariokart64n (coreynguyen)." << std::endl;
	Sleep(100);
}

void OptionsMenu()
{
	bool isExited = false;

	while (!isExited)
	{
		system("cls");

		std::cout << std::endl << " Your settings:" << std::endl;
		std::cout << " - Backup archives [";
		if (char(backUp) == 0)
			std::cout << "NOT SET]" << std::endl;
		else
			std::cout << char(backUp) << "]" << std::endl;

		std::cout << " - Change start screen [";
		if (char(screen) == 0)
			std::cout << "NOT SET]" << std::endl;
		else
			std::cout << char(screen) << "]" << std::endl;

		for (int i = 0; i < names.size(); i++)
		{
			std::cout << " - " << names[i] << " [";

			if (namesID[i] == 0)
				std::cout << "N";
			else
				std::cout << "Y";
			std::cout << "]" << std::endl;
		}

		std::cout << std::endl;
		std::cout << " |--------------[Options]--------------|" << std::endl;
		std::cout << " |1.             Backups               |" << std::endl;
		std::cout << " |2.           Start Screen            |" << std::endl;
		std::cout << " |3.        Extracted archives         |" << std::endl;
		std::cout << " |4.              Back                 |" << std::endl;
		std::cout << std::endl << " > ";

		char choice;
		std::cin >> choice;

		switch (choice)
		{

		case 49: 
		{
			bool exit = false;
			while (!exit)
			{
				DisplayInfo("Do you need to create backup files?\n Press Y or N.\n", true);

				backUp = _getch();

				if (backUp > 90)
					backUp -= 32;

				if (backUp == 89 || backUp == 78)
					exit = true;
				else
					DisplayInfo("Data entered incorrectly.", true);
			}
			SaveOptions();
			break;
		}

		case 50: 
		{
			bool exit = false;
			while (!exit)
			{
				DisplayInfo("Do you need to change start screen?\n Press Y or N.\n", true);

				screen = _getch();

				if (screen > 90)
					screen -= 32;

				if (screen == 89 || screen == 78)
					exit = true;
				else
					DisplayInfo("Data entered incorrectly.", true);
			}
			SaveOptions();
			break; 
		}

		case 51:
		{
			for (int i = 0; i < names.size(); i++)
			{
				bool exit = false;
				while (!exit)
				{
					DisplayInfo("Do you want to work with " + names[i] + "?\n Press Y or N.\n", true);

					int ch = _getch();

					if (ch > 90)
						ch -= 32;

					switch (ch)
					{
					case 89:
						namesID[i] = 1;
						exit = true;
						break;
					case 78:
						namesID[i] = 0;
						exit = true;
						break;
					default:
						DisplayInfo("Data entered incorrectly.", true);
						break;
					}
				}
			}
			SaveOptions();
			break;
		}

		case 52:
		{
			isExited = true;
			break;
		}

		default:
		{
			DisplayInfo("Enter the correct value.", true);
			break;
		}
		}
	}

	system("cls");
}

int main(int argc, char* argv[])
{
	DisplayInfo("Welcome to...", true);
	bool isExited = false;

	while (!isExited)
	{
		LoadOptions();

		for (int i = 0; i < names.size(); i++)
			if (fs::exists(currentPath + "\\" + names[i] + ".temp"))
				fs::remove(currentPath + "\\" + names[i] + ".temp");

		std::cout << std::endl;
		std::cout << " |-------[Beetle Crazy Cup Tool]-------|" << std::endl;
		std::cout << " |1. Extract all files from archives   |" << std::endl;
		std::cout << " |2.       Create new archives         |" << std::endl;
		std::cout << " |3.             obj2inf               |" << std::endl;
		std::cout << " |4.             inf2obj               |" << std::endl;
		std::cout << " |5.              Help                 |" << std::endl;
		std::cout << " |6.             Options               |" << std::endl;
		std::cout << " |7.              Exit                 |" << std::endl;
		std::cout << std::endl << " > ";

		char choice;
		std::cin >> choice;

		switch (choice)
		{

		case 49:
			Extract();
			break;

		case 50:
			PackBack();
			break;

		case 51:
			Obj2Inf();
			break;

		case 52:
			Inf2Obj();
			break;

		case 53:
			Help();
			break;

		case 54:
			OptionsMenu();
			break;

		case 55:
			isExited = true;
			break;
		
		default:
			DisplayInfo("Enter the correct value.", true);
			break;
		}
	}

	return 0;
}