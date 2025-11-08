"""Tests for project_cache module."""

import unittest
from src.discovery.project_cache import FileInfo, ProjectFileCache
from test.common.constants.discovery_test_constants import (
    TEST_ABSOLUTE_PATH, TEST_RELATIVE_PATH, TEST_EMPTY_PATH,
    MOCK_AUTOWIRE_PATH, MOCK_PROVIDER_PATH, MOCK_BOTH_PATH, MOCK_NONE_PATH,
    MOCK_NEW_AUTOWIRE_PATH, MOCK_NEW_PROVIDER_PATH, MOCK_NEW_BOTH_PATH, MOCK_NEW_NONE_PATH,
    MOCK_AUTOWIRE_CONTENT, MOCK_PROVIDER_CONTENT, MOCK_BOTH_CONTENT, MOCK_NONE_CONTENT,
    MOCK_UPDATED_CONTENT, TEST_NONEXISTENT_PATH, create_sample_file_info_dict
)


class TestFileInfo(unittest.TestCase):
    """Test cases for FileInfo dataclass."""
    
    def test_valid_absolute_path(self):
        """Test FileInfo with valid absolute path."""
        file_info = FileInfo(
            file_path=TEST_ABSOLUTE_PATH,
            file_content=MOCK_AUTOWIRE_CONTENT,
            has_autowire=True,
            has_provider=False
        )
        
        expected = (TEST_ABSOLUTE_PATH, MOCK_AUTOWIRE_CONTENT, True, False)
        actual = (file_info.file_path, file_info.file_content, file_info.has_autowire, file_info.has_provider)
        
        self.assertEqual(expected, actual)
    
    def test_relative_path_raises_error(self):
        """Test FileInfo raises ValueError for relative path."""
        with self.assertRaises(ValueError):
            FileInfo(
                file_path=TEST_RELATIVE_PATH,
                file_content=MOCK_NONE_CONTENT,
                has_autowire=False,
                has_provider=False
            )
    
    def test_empty_path_raises_error(self):
        """Test FileInfo raises ValueError for empty path."""
        with self.assertRaises(ValueError):
            FileInfo(
                file_path=TEST_EMPTY_PATH,
                file_content=MOCK_NONE_CONTENT,
                has_autowire=False,
                has_provider=False
            )


class TestProjectFileCache(unittest.TestCase):
    """Test cases for ProjectFileCache class."""
    
    def setUp(self):
        """Set up test fixtures before each test method."""
        self.sample_files = create_sample_file_info_dict()
        
        self.cache = ProjectFileCache(
            all_files=self.sample_files,
            autowire_files=[MOCK_AUTOWIRE_PATH, MOCK_BOTH_PATH],
            provider_files=[MOCK_PROVIDER_PATH, MOCK_BOTH_PATH]
        )
    
    def test_initialization(self):
        """Test cache initialization."""
        expected = (4, 2, 2)
        actual = (len(self.cache.get_all_files()), len(self.cache.autowire_files), len(self.cache.provider_files))
        
        self.assertEqual(expected, actual)
    
    def test_autowire_files_property(self):
        """Test autowire_files property returns correct files."""
        autowire_files = self.cache.autowire_files
        expected_in = [MOCK_AUTOWIRE_PATH, MOCK_BOTH_PATH]
        expected_not_in = [MOCK_PROVIDER_PATH, MOCK_NONE_PATH]
        
        for path in expected_in:
            self.assertIn(path, autowire_files)
        for path in expected_not_in:
            self.assertNotIn(path, autowire_files)
    
    def test_provider_files_property(self):
        """Test provider_files property returns correct files."""
        provider_files = self.cache.provider_files
        expected_in = [MOCK_PROVIDER_PATH, MOCK_BOTH_PATH]
        expected_not_in = [MOCK_AUTOWIRE_PATH, MOCK_NONE_PATH]
        
        for path in expected_in:
            self.assertIn(path, provider_files)
        for path in expected_not_in:
            self.assertNotIn(path, provider_files)
    
    def test_get_file_content(self):
        """Test get_file_content method."""
        autowire_content = self.cache.get_file_content(MOCK_AUTOWIRE_PATH)
        provider_content = self.cache.get_file_content(MOCK_PROVIDER_PATH)
        
        expected = (MOCK_AUTOWIRE_CONTENT, MOCK_PROVIDER_CONTENT)
        actual = (autowire_content, provider_content)
        
        self.assertEqual(expected, actual)
    
    def test_get_file_content_missing_file(self):
        """Test get_file_content raises KeyError for missing file."""
        with self.assertRaises(KeyError):
            self.cache.get_file_content(TEST_NONEXISTENT_PATH)
    
    def test_contains_operator(self):
        """Test __contains__ operator."""
        expected_in = [MOCK_AUTOWIRE_PATH, MOCK_PROVIDER_PATH]
        expected_not_in = [TEST_NONEXISTENT_PATH]
        
        for path in expected_in:
            self.assertIn(path, self.cache)
        for path in expected_not_in:
            self.assertNotIn(path, self.cache)
    
    def test_add_file_autowire_only(self):
        """Test adding file with only autowire annotation."""
        new_file = FileInfo(MOCK_NEW_AUTOWIRE_PATH, MOCK_AUTOWIRE_CONTENT, True, False)
        self.cache.add_file(new_file)
        
        expected = (True, True, False, MOCK_AUTOWIRE_CONTENT)
        actual = (
            MOCK_NEW_AUTOWIRE_PATH in self.cache,
            MOCK_NEW_AUTOWIRE_PATH in self.cache.autowire_files,
            MOCK_NEW_AUTOWIRE_PATH in self.cache.provider_files,
            self.cache.get_file_content(MOCK_NEW_AUTOWIRE_PATH)
        )
        
        self.assertEqual(expected, actual)
    
    def test_add_file_provider_only(self):
        """Test adding file with only provider annotation."""
        new_file = FileInfo(MOCK_NEW_PROVIDER_PATH, MOCK_PROVIDER_CONTENT, False, True)
        self.cache.add_file(new_file)
        
        expected = (True, False, True)
        actual = (
            MOCK_NEW_PROVIDER_PATH in self.cache,
            MOCK_NEW_PROVIDER_PATH in self.cache.autowire_files,
            MOCK_NEW_PROVIDER_PATH in self.cache.provider_files
        )
        
        self.assertEqual(expected, actual)
    
    def test_add_file_both_annotations(self):
        """Test adding file with both annotations."""
        new_file = FileInfo(MOCK_NEW_BOTH_PATH, MOCK_BOTH_CONTENT, True, True)
        self.cache.add_file(new_file)
        
        expected = (True, True, True)
        actual = (
            MOCK_NEW_BOTH_PATH in self.cache,
            MOCK_NEW_BOTH_PATH in self.cache.autowire_files,
            MOCK_NEW_BOTH_PATH in self.cache.provider_files
        )
        
        self.assertEqual(expected, actual)
    
    def test_add_file_no_annotations(self):
        """Test adding file with no annotations."""
        new_file = FileInfo(MOCK_NEW_NONE_PATH, MOCK_NONE_CONTENT, False, False)
        self.cache.add_file(new_file)
        
        expected = (True, False, False)
        actual = (
            MOCK_NEW_NONE_PATH in self.cache,
            MOCK_NEW_NONE_PATH in self.cache.autowire_files,
            MOCK_NEW_NONE_PATH in self.cache.provider_files
        )
        
        self.assertEqual(expected, actual)
    
    def test_add_file_prevents_duplicates(self):
        """Test adding file prevents duplicates in annotation lists."""
        existing_file = FileInfo(MOCK_AUTOWIRE_PATH, MOCK_UPDATED_CONTENT, True, False)
        self.cache.add_file(existing_file)
        
        autowire_count = self.cache.autowire_files.count(MOCK_AUTOWIRE_PATH)
        updated_content = self.cache.get_file_content(MOCK_AUTOWIRE_PATH)
        
        expected = (1, MOCK_UPDATED_CONTENT)
        actual = (autowire_count, updated_content)
        
        self.assertEqual(expected, actual)