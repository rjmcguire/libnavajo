// This file is distributed under GPLv3 licence
// Author: Gorelov Grigory (gorelov@grigory.info)
//
// Contacts and other info are on the WEB page:  grigory.info/MPFDParser

#include "MPFDParser/Field.h"
#include "MPFDParser/Parser.h"

pthread_mutex_t fileCreation_mutex = PTHREAD_MUTEX_INITIALIZER;

MPFD::Field::Field() {
    type = 0;

}

MPFD::Field::~Field() {

    if (type == FileType) {
        if (file.is_open()) {
            file.close();
            remove((TempDir + "/" + TempFile).c_str());
        }

    }

}

void MPFD::Field::SetType(int type) {
    if ((type == TextType) || (type == FileType)) {
        this->type = type;
    } else {
        throw MPFD::Exception("Trying to set type of field, but type is incorrect.");
    }

}

int MPFD::Field::GetType() {
    if (type > 0) {
        return type;
    } else {
        throw MPFD::Exception("Trying to get type of field, but no type was set.");
    }
}

void MPFD::Field::AcceptSomeData(char *data, long length) {
    if (type == TextType) {
        const size_t FieldContentLength = FieldContent.size();
        const size_t newFieldContentLength = FieldContentLength + length + 1;
        FieldContent.resize(newFieldContentLength);

        memcpy(&FieldContent[0] + FieldContentLength, data, length);

        FieldContent[newFieldContentLength-1] = 0;
    } else if (type == FileType) {
        if (WhereToStoreUploadedFiles == Parser::StoreUploadedFilesInFilesystem) {
            if (TempDir.length() > 0) {
	      
                if (!file.is_open()) {
pthread_mutex_lock( &fileCreation_mutex );
                    int i = 1;
                    std::ifstream testfile;
                    std::string tempfile;
                    do {
                        if (testfile.is_open()) {
                            testfile.close();
                        }

                        std::stringstream ss;
                        ss << "MPFD_Temp_" << i;
                        TempFile = ss.str();

                        tempfile = TempDir + "/" + TempFile;

                        testfile.open(tempfile.c_str(), std::ios::in);
                        i++;
                    } while (testfile.is_open());

                    file.open(tempfile.c_str(), std::ios::out | std::ios::binary | std::ios_base::trunc);
pthread_mutex_unlock( &fileCreation_mutex );
                }

                if (file.is_open()) {
                    file.write(data, length);
                    file.flush();
                } else {
                    throw Exception(std::string("Cannot write to file ") + TempDir + "/" + TempFile);
                }
            } else {
                throw MPFD::Exception("Trying to AcceptSomeData for a file but no TempDir is set.");
            }
        } else { // If files are stored in memory
	    const size_t FieldContentLength = FieldContent.size();
	    const size_t newFieldContentLength = FieldContentLength + length;
            FieldContent.resize(newFieldContentLength);
            memcpy(&FieldContent[0] + FieldContentLength, data, length);
        }
    } else {
        throw MPFD::Exception("Trying to AcceptSomeData but no type was set.");
    }
}

void MPFD::Field::SetTempDir(std::string dir) {
    TempDir = dir;
}

unsigned long MPFD::Field::GetFileContentSize() {
    if (type == 0) {
        throw MPFD::Exception("Trying to get file content size, but no type was set.");
    } else {
        if (type == FileType) {
            if (WhereToStoreUploadedFiles == Parser::StoreUploadedFilesInMemory) {
	        return FieldContent.size();
            } else {
                throw MPFD::Exception("Trying to get file content size, but uploaded files are stored in filesystem.");
            }
        } else {
            throw MPFD::Exception("Trying to get file content size, but the type is not file.");
        }
    }
}

char * MPFD::Field::GetFileContent() {
    if (type == 0) {
        throw MPFD::Exception("Trying to get file content, but no type was set.");
    } else {
        if (type == FileType) {
            if (WhereToStoreUploadedFiles == Parser::StoreUploadedFilesInMemory) {
                return &FieldContent[0];
            } else {
                throw MPFD::Exception("Trying to get file content, but uploaded files are stored in filesystem.");
            }
        } else {
            throw MPFD::Exception("Trying to get file content, but the type is not file.");
        }
    }
}

std::string MPFD::Field::GetTextTypeContent() {
    if (type == 0) {
        throw MPFD::Exception("Trying to get text content of the field, but no type was set.");
    } else {
        if (type != TextType) {
            throw MPFD::Exception("Trying to get content of the field, but the type is not text.");
        } else {
	    if (FieldContent.empty()) {
                return std::string();
            } else {
                return std::string(&FieldContent[0]);
            }
        }
    }
}

std::string MPFD::Field::GetTempFileName() {
    if (type == 0) {
        throw MPFD::Exception("Trying to get file temp name, but no type was set.");
    } else {
        if (type == FileType) {
            if (WhereToStoreUploadedFiles == Parser::StoreUploadedFilesInFilesystem) {
                return std::string(TempDir + "/" + TempFile);
            } else {
                throw MPFD::Exception("Trying to get file temp name, but uplaoded files are stored in memory.");
            }
        } else {
            throw MPFD::Exception("Trying to get file temp name, but the type is not file.");
        }
    }
}

std::string MPFD::Field::GetFileName() {
    if (type == 0) {
        throw MPFD::Exception("Trying to get file name, but no type was set.");
    } else {
        if (type == FileType) {
            return FileName;
        } else {
            throw MPFD::Exception("Trying to get file name, but the type is not file.");
        }
    }
}

void MPFD::Field::SetFileName(std::string name) {
    FileName = name;

}

void MPFD::Field::SetUploadedFilesStorage(int where) {
    WhereToStoreUploadedFiles = where;
}

void MPFD::Field::SetFileContentType(std::string type) {
    FileContentType = type;
}

std::string MPFD::Field::GetFileMimeType() {
    if (type == 0) {
        throw MPFD::Exception("Trying to get mime type of file, but no type was set.");
    } else {
        if (type != FileType) {
            throw MPFD::Exception("Trying to get mime type of the field, but the type is not File.");
        } else {
            return std::string(FileContentType);
        }
    }
}
