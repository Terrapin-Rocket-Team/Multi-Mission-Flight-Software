#include <unity.h>
#include "../../lib/NativeTestMocks/NativeTestHelper.h"
#include "../../lib/NativeTestMocks/MockLoggingBackend.h"

void setUp(void) {}
void tearDown(void) {}

void test_mock_open_behavior()
{
    mmfs::LoggingBackendMock mockBackend;
    mockBackend.begin();

    const char *filename = "openfile.txt";
    const char *data = "abc";

    // Open and write
    mmfs::LoggingBackendFile *file1 = mockBackend.open(filename);
    TEST_ASSERT_NOT_NULL(file1);
    file1->write(data, strlen(data));
    file1->close();

    // Open again and confirm contents are still there
    mmfs::LoggingBackendFile *file2 = mockBackend.open(filename);
    TEST_ASSERT_NOT_NULL(file2);

    char buffer[64] = {0};
    int read = file2->readBytes(buffer, sizeof(buffer));
    file2->close();

    TEST_ASSERT_EQUAL(strlen(data), read);
    TEST_ASSERT_EQUAL_STRING_LEN(data, buffer, read);

    // Confirm file existence
    TEST_ASSERT_TRUE(mockBackend.exists(filename));
}

void test_mock_write_and_read()
{
    mmfs::LoggingBackendMock mockBackend;
    mockBackend.begin();

    const char *filename = "testfile.txt";
    const char *testData = "Hello, MMFS!";

    mmfs::LoggingBackendFile *file = mockBackend.open(filename);
    TEST_ASSERT_NOT_NULL(file);
    size_t written = file->write((const uint8_t *)testData, strlen(testData));
    TEST_ASSERT_EQUAL(strlen(testData), written);
    file->close();

    file = mockBackend.open(filename);
    TEST_ASSERT_NOT_NULL(file);
    char buffer[64] = {0};
    int read = file->readBytes(buffer, sizeof(buffer));
    file->close();

    TEST_ASSERT_EQUAL(strlen(testData), read);
    TEST_ASSERT_EQUAL_STRING_LEN(testData, buffer, read);
}

void test_mock_seek_and_overwrite()
{
    mmfs::LoggingBackendMock mockBackend;
    mockBackend.begin();

    const char *filename = "seekfile.txt";
    const char *initialData = "ABCDEFG";
    const char *overwriteData = "123";

    mmfs::LoggingBackendFile *file = mockBackend.open(filename);
    file->write(initialData, strlen(initialData));
    file->seek(2); // move to index 2
    file->write(overwriteData, strlen(overwriteData));
    file->seek(0);

    char buffer[64] = {0};
    int read = file->readBytes(buffer, sizeof(buffer));
    file->close();

    TEST_ASSERT_EQUAL_STRING_LEN("AB123FG", buffer, 7);
}

void test_mock_remove_and_exists()
{
    mmfs::LoggingBackendMock mockBackend;
    mockBackend.begin();

    const char *filename = "delete_me.txt";
    mmfs::LoggingBackendFile *file = mockBackend.open(filename);
    file->write("data", 4);
    file->close();

    TEST_ASSERT_TRUE(mockBackend.exists(filename));
    TEST_ASSERT_TRUE(mockBackend.remove(filename));
    TEST_ASSERT_FALSE(mockBackend.exists(filename));
}

void test_mock_get_mock_file_data()
{
    mmfs::LoggingBackendMock mockBackend;
    mockBackend.begin();

    const char *existingFile = "real.txt";
    const char *missingFile = "ghost.txt";
    const char *content = "Test123";

    // Create and write to a real file
    mmfs::LoggingBackendFile *file = mockBackend.open(existingFile);
    TEST_ASSERT_NOT_NULL(file);
    file->write(content, strlen(content));
    file->close();

    // Retrieve the backing file data directly
    mmfs::MockFileData *fileData = mockBackend.getMockFileData(existingFile);
    TEST_ASSERT_NOT_NULL(fileData);
    TEST_ASSERT_EQUAL(strlen(content), fileData->size);
    TEST_ASSERT_EQUAL_STRING_LEN(content, fileData->arr, fileData->size);

    // Try getting a non-existent file
    mmfs::MockFileData *missingData = mockBackend.getMockFileData(missingFile);
    TEST_ASSERT_NULL(missingData);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_mock_open_behavior);
    RUN_TEST(test_mock_write_and_read);
    RUN_TEST(test_mock_seek_and_overwrite);
    RUN_TEST(test_mock_remove_and_exists);
    RUN_TEST(test_mock_get_mock_file_data);

    UNITY_END();
}
