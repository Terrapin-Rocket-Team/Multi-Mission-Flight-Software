#include <unity.h>
#include "../../lib/NativeTestMocks/NativeTestHelper.h"

// include other headers you need to test here

// #include "../../src/......"
#include "../../src/RecordData/psram.h"
#include "../../src/RecordData/PSRAMFile.h"

using namespace mmfs;

// ---

// Set up and global variables or mocks for testing here

char *fakepsram = nullptr;

PSRAM *psram;
// ---

// These two functions are called before and after each test function, and are required in unity, even if empty.
void setUp(void)
{
    // set stuff up before each test here, if needed
}

void tearDown(void)
{
    // clean stuff up after each test here, if needed
}
// ---

// Test functions must be void and take no arguments, put them here

// void test_function_name() {
//     TEST_ASSERT_EQUAL(expected, actual);
//     TEST_ASSERT_EQUAL_FLOAT(expected, actual);
// }

void test_PSRAMFile_constructor()
{
    PSRAMFile file("testfile", psram);
    TEST_ASSERT_EQUAL_STRING("testfile", file.getName());
    TEST_ASSERT_EQUAL(0, file.getSize());
}

void test_PSRAM_init()
{
    TEST_ASSERT_TRUE(psram->init());
    TEST_ASSERT_TRUE(psram->isReady());
    TEST_ASSERT_EQUAL(ONE_MB * external_psram_size, psram->getSize());
}

void test_PSRAM_open_create()
{
    PSRAMFile *file = psram->open("testfile", F_WRITE, true);
    TEST_ASSERT_NOT_NULL(file);
    TEST_ASSERT_EQUAL_STRING("testfile", file->getName());
    TEST_ASSERT_EQUAL(0, file->getSize());

    PSRAMFile *file2 = psram->open("testfile", false);
    TEST_ASSERT_EQUAL(file, file2);

    PSRAMFile *file3 = psram->open(0, F_WRITE);
    TEST_ASSERT_EQUAL(file, file3);
}

void test_PSRAM_open_no_create()
{
    PSRAMFile *file = psram->open("testfile2", F_WRITE, false);
    TEST_ASSERT_NULL(file);

    PSRAMFile *file2 = psram->open(2, F_WRITE);
    TEST_ASSERT_NULL(file2);
}

void test_PSRAM_writeFile()
{
    PSRAMFile *file = psram->open("testfile", F_WRITE | F_READ, false);
    TEST_ASSERT_NOT_NULL(file);

    char data[] = "Hello, world!";
    file->print(data);

    TEST_ASSERT_EQUAL(13, file->getSize());
}

void test_PSRAM_write2()
{
    PSRAMFile *file = psram->open("testfile", F_APPEND | F_READ, false);
    TEST_ASSERT_NOT_NULL(file);

    char data[] = "Hello, world!";
    file->print(data);

    TEST_ASSERT_EQUAL(26, file->getSize());
}

void test_PSRAM_2ndFile()
{
    PSRAMFile *file = psram->open("testfile2", F_APPEND | F_READ, true);
    TEST_ASSERT_NOT_NULL(file);

    char data[] = "Hello, world!";
    file->print(data);

    TEST_ASSERT_EQUAL(13, file->getSize());
}

void test_PSRAM_multiple_clusters()
{
    PSRAMFile *file = psram->open("testfile", F_APPEND | F_READ, false);
    TEST_ASSERT_NOT_NULL(file);

    char data[256];
    for (int i = 0; i < 256; i++)
    {
        if (i % 10 == 0)
            data[i] = '\n';
        else
            data[i] = 'a' + (i % 26);
    }
    data[255] = '\0';
    file->print(data);

    TEST_ASSERT_EQUAL(26 + 255, file->getSize()); // null terminator is not put in file
}

void test_PSRAM_readFile()
{
    PSRAMFile *file = psram->open("testfile", F_READ, false);
    TEST_ASSERT_NOT_NULL(file);

    char data[282]; // 26 + 256

    strcpy(data, "Hello, world!Hello, world!");

    for (int i = 0; i + 26 < 282; i++)
    {
        if (i % 10 == 0)
            data[i + 26] = '\n';
        else
            data[i + 26] = 'a' + (i % 26);
    }
    data[281] = '\0';
    char readData[282];
    int i = file->read(readData, 282);
    readData[file->getSize()] = '\0';


    TEST_ASSERT_EQUAL_STRING(data, readData);
    TEST_ASSERT_EQUAL(281, i);

    i = file->read(readData, 282);

    TEST_ASSERT_EQUAL(0, i);

}

void test_PSRAM_read_file2()
{
    PSRAMFile *file = psram->open("testfile2", F_READ, false);
    TEST_ASSERT_NOT_NULL(file);

    char data[] = "Hello, world!";
    char readData[14];
    file->read(readData, file->getSize());
    readData[file->getSize()] = '\0';

    TEST_ASSERT_EQUAL_STRING(data, readData);
}

void test_PSRAM_read_next_cluster()
{
    PSRAMFile *file = psram->open("testfile", F_READ, false);
    TEST_ASSERT_NOT_NULL(file);

    char data[256];
    strcpy(data, "Hello, world!Hello, world!");

    for (int i = 0; i + 26 < 256; i++)
    {
        if (i % 10 == 0)
            data[i + 26] = '\n';
        else
            data[i + 26] = 'a' + (i % 26);
    }

    int size;
    char *cluster = psram->readNextFileCluster(*file, size);
    TEST_ASSERT_EQUAL(256, size);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(data, cluster, 256);

    cluster = psram->readNextFileCluster(*file, size);
    TEST_ASSERT_EQUAL(25, size);
    memcpy(data, "\nxyzabcdef\nhijklmnop\nrstu", 26);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(data, cluster, 25);
}

void test_PSRAM_3rdFile()
{
    PSRAMFile *file = psram->open("testfile3", F_WRITE | F_READ, true);
    TEST_ASSERT_NOT_NULL(file);

    char data[] = "Hello, world!";
    file->print(data);

    TEST_ASSERT_EQUAL(13, file->getSize());

    char data2[14];
    file->restart();
    file->read(data2, file->getSize());
    data2[file->getSize()] = '\0';

    TEST_ASSERT_EQUAL_CHAR_ARRAY(data, data2, 14);
}

void test_PSRAM_read_next_cluster2()
{
    PSRAMFile *file = psram->open("testfile3", F_READ, false);
    TEST_ASSERT_NOT_NULL(file);

    char data[] = "Hello, world!";
    char readData[14];
    file->read(readData, 13);
    readData[13] = '\0';

    TEST_ASSERT_EQUAL_STRING(data, readData);

    int size;
    char *cluster = psram->readNextFileCluster(*file, size);
    TEST_ASSERT_EQUAL(13, size);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(data, cluster, 13);

    cluster = psram->readNextFileCluster(*file, size);
    TEST_ASSERT_EQUAL(0, size);
    TEST_ASSERT_NULL(cluster);
}

void test_file_create_no_space()
{
    PSRAMFile *file = psram->open("testfile4", F_WRITE, true);
    TEST_ASSERT_NULL(file);
}

void test_write_file_no_space()
{
    PSRAMFile *file = psram->open("testfile3", F_APPEND, false);
    TEST_ASSERT_NOT_NULL(file);
    char data[256];
    for (int i = 0; i < 256; i++)
    {
        if (i % 10 == 0)
            data[i] = '\n';
        else
            data[i] = 'a' + (i % 26);
    }
    data[255] = '\0';
    file->print(data);

    TEST_ASSERT_EQUAL(256, file->getSize());
}

void test_file_open_close()
{
    PSRAMFile *file = psram->open("testfile2", F_READ, false);
    TEST_ASSERT_NOT_NULL(file);
    TEST_ASSERT_EQUAL(13, file->getSize());

    file->close();
    TEST_ASSERT_FALSE(file->isOpen());
    char data[14];
    data[0] = '\0';
    file->read(data, file->getSize());
    data[file->getSize()] = '\0';
    TEST_ASSERT_EQUAL_STRING("", data); // can't read from a closed file

    file->open(F_READ); // reopen the file
    TEST_ASSERT_TRUE(file->isOpen());
    file->read(data, file->getSize());
    data[file->getSize()] = '\0';
    TEST_ASSERT_EQUAL_STRING("Hello, world!", data);
}

void test_seek_read()
{
    PSRAMFile *file = psram->open("testfile2", F_READ, false);
    TEST_ASSERT_NOT_NULL(file);
    TEST_ASSERT_EQUAL(13, file->getSize());

    char data[file->getSize() + 1];
    file->seek(6);
    file->read(data, file->getSize() - 6);
    data[file->getSize() - 6] = '\0';
    TEST_ASSERT_EQUAL_STRING(" world!", data);

    file->seek(6, F_SEEK_END);
    int rd = file->read(data, file->getSize());
    data[rd] = '\0';
    TEST_ASSERT_EQUAL_STRING("world!", data);

    // NOT IMPLEMENTED
    // file->seek(-9, F_SEEK_CUR);
    // rd = file->read(data, file->getSize());
    // data[rd] = '\0';
    // TEST_ASSERT_EQUAL_STRING("lo, world!", data);
}

void test_seek_write()
{
    PSRAMFile *file = psram->open("testfile2", F_WRITE | F_READ, false);
    TEST_ASSERT_NOT_NULL(file);
    TEST_ASSERT_EQUAL(13, file->getSize());

    char data[] = "Hello, world!";
    file->print(data);

    file->seek(7);
    char data2[] = "everyone!";
    file->print(data2);

    TEST_ASSERT_EQUAL(16, file->getSize());

    char readData[file->getSize() + 1];
    file->restart();
    file->read(readData, file->getSize());
    readData[file->getSize()] = '\0';
    TEST_ASSERT_EQUAL_STRING("Hello, everyone!", readData);
}

// ---
// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    ONE_MB = 1024; // temp make this one KB for testing to limit amount of memory to verify
    fakepsram = new char[ONE_MB * external_psram_size];
    PSRAM_STARTING_ADDRESS = (uintptr_t)fakepsram;
    PSRAM_CLUSTER_SIZE = 256; // in bytes

    psram = new PSRAM();

    // Add your tests here
    // RUN_TEST(test_function_name); // no parentheses after function name

    // THE ORDER OF THESE TESTS MAY MATTER. BE CAREFUL WHEN MOVING THEM AROUND OR ADDING NEW TESTS

    RUN_TEST(test_PSRAMFile_constructor);
    RUN_TEST(test_PSRAM_init);
    RUN_TEST(test_PSRAM_open_create);
    RUN_TEST(test_PSRAM_open_no_create);
    RUN_TEST(test_PSRAM_writeFile);
    RUN_TEST(test_PSRAM_write2);
    RUN_TEST(test_PSRAM_2ndFile);
    RUN_TEST(test_PSRAM_multiple_clusters);
    RUN_TEST(test_PSRAM_readFile);
    RUN_TEST(test_PSRAM_read_file2);
    RUN_TEST(test_PSRAM_read_next_cluster);
    RUN_TEST(test_PSRAM_3rdFile);
    RUN_TEST(test_PSRAM_read_next_cluster2);
    RUN_TEST(test_file_create_no_space);
    RUN_TEST(test_write_file_no_space);
    RUN_TEST(test_file_open_close);
    RUN_TEST(test_seek_read);
    RUN_TEST(test_seek_write);

    UNITY_END();
}
// ---