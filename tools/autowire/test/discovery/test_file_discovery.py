"""Tests for file_discovery module."""

import unittest
import tempfile
import os
from unittest.mock import patch
from src.discovery.file_discovery import FileDiscovery
from src.discovery.project_cache import FileInfo, ProjectFileCache
from test.common.constants.discovery_test_constants import (
    AUTOWIRE_CONTENT, PROVIDER_CONTENT, EXPECTED_BASIC_MOCK_RESULTS, EXPECTED_GREP_MOCK_RESULTS,
    EXPECTED_MIXED_MOCK_RESULTS, build_mock_results_dict,
    MULTI_FILE_NAMES, MULTI_FILE_CONTENTS, EXPECTED_MULTI_FILE_RESULTS,
    SRC_SUBDIR, TEST_CONTENT_LITERAL, MULTI_FILE_AUTOWIRE_KEYS, MULTI_FILE_PROVIDER_KEYS,
    EMPTY_CACHE_ARGS, FILE_READ_ERROR_MOCK, create_temp_directory, cleanup_temp_directory,
    create_standard_test_files, create_multi_test_files
)


class TestFileDiscovery(unittest.TestCase):
    """Test cases for FileDiscovery class."""
    
    def setUp(self):
        """Set up test fixtures before each test method."""
        self.temp_dir = create_temp_directory()
        self.files = create_standard_test_files(self.temp_dir, SRC_SUBDIR)
        # Map 'none' to 'plain' for consistency with test expectations
        self.files['plain'] = self.files['none']
    
    def tearDown(self):
        """Clean up after each test method."""
        cleanup_temp_directory(self.temp_dir)
    
    def test_initialization(self):
        """Test FileDiscovery initialization."""
        discovery = FileDiscovery(self.temp_dir)
        
        expected = (self.temp_dir, [self.temp_dir], True)
        actual = (discovery.root_path, discovery.scanner.filepaths, discovery.scanner is not None)
        
        self.assertEqual(expected, actual)
    
    def test_get_annotated_files_basic(self):
        """Test get_annotated_files method basic functionality."""
        discovery = FileDiscovery(self.temp_dir)
        
        # Build mock results using file paths
        mock_results = build_mock_results_dict(self.files, EXPECTED_BASIC_MOCK_RESULTS)
        
        with patch.object(discovery.scanner, 'scan', return_value=mock_results):
            actual = discovery.get_annotated_files()
            
            # Build expected cache using mock result contents
            expected_files = {
                self.files['autowire']: FileInfo(self.files['autowire'], EXPECTED_BASIC_MOCK_RESULTS['autowire'][0], True, False),
                self.files['provider']: FileInfo(self.files['provider'], EXPECTED_BASIC_MOCK_RESULTS['provider'][0], False, True),
                self.files['both']: FileInfo(self.files['both'], EXPECTED_BASIC_MOCK_RESULTS['both'][0], True, True),
                self.files['plain']: FileInfo(self.files['plain'], EXPECTED_BASIC_MOCK_RESULTS['plain'][0], False, False)
            }
            expected = ProjectFileCache(
                root_path=self.temp_dir,
                all_files=expected_files,
                autowire_files=[self.files['autowire'], self.files['both']],
                provider_files=[self.files['provider'], self.files['both']]
            )
            
            self.assertEqual(expected, actual)
    
    def test_get_annotated_files_with_none_content(self):
        """Test get_annotated_files when scanner returns None content (grep mode)."""
        discovery = FileDiscovery(self.temp_dir)
        
        # Build mock results for grep mode
        mock_results = build_mock_results_dict(self.files, EXPECTED_GREP_MOCK_RESULTS)
        
        with patch.object(discovery.scanner, 'scan', return_value=mock_results):
            actual = discovery.get_annotated_files()
            
            # Build expected cache with file contents read from disk
            expected_files = {
                self.files['autowire']: FileInfo(self.files['autowire'], AUTOWIRE_CONTENT, True, False),
                self.files['provider']: FileInfo(self.files['provider'], PROVIDER_CONTENT, False, True)
            }
            expected = ProjectFileCache(
                root_path=self.temp_dir,
                all_files=expected_files,
                autowire_files=[self.files['autowire']],
                provider_files=[self.files['provider']]
            )
            
            self.assertEqual(expected, actual)
    
    def test_get_annotated_files_file_read_error(self):
        """Test get_annotated_files when file reading fails."""
        discovery = FileDiscovery(self.temp_dir)
        
        # Mock scanner returning None content for non-existent file
        mock_results = FILE_READ_ERROR_MOCK
        
        with patch.object(discovery.scanner, 'scan', return_value=mock_results):
            # Should raise exception when trying to read non-existent file
            with self.assertRaises(FileNotFoundError):
                discovery.get_annotated_files()
    
    def test_get_annotated_files_empty_results(self):
        """Test get_annotated_files with empty scanner results."""
        discovery = FileDiscovery(self.temp_dir)
        
        with patch.object(discovery.scanner, 'scan', return_value={}):
            actual = discovery.get_annotated_files()
            
            expected = ProjectFileCache(self.temp_dir, **EMPTY_CACHE_ARGS)
            
            self.assertEqual(expected, actual)
    
    def test_get_annotated_files_mixed_content(self):
        """Test get_annotated_files with mix of None and actual content."""
        discovery = FileDiscovery(self.temp_dir)
        
        # Build mixed mock results
        mock_results = build_mock_results_dict(self.files, EXPECTED_MIXED_MOCK_RESULTS)
        
        with patch.object(discovery.scanner, 'scan', return_value=mock_results):
            actual = discovery.get_annotated_files()
            
            # Build expected cache with mixed content sources
            expected_files = {
                self.files['autowire']: FileInfo(self.files['autowire'], AUTOWIRE_CONTENT, True, False),
                self.files['provider']: FileInfo(self.files['provider'], EXPECTED_MIXED_MOCK_RESULTS['provider'][0], False, True),
                self.files['plain']: FileInfo(self.files['plain'], EXPECTED_MIXED_MOCK_RESULTS['plain'][0], False, False)
            }
            expected = ProjectFileCache(
                root_path=self.temp_dir,
                all_files=expected_files,
                autowire_files=[self.files['autowire']],
                provider_files=[self.files['provider']]
            )
            
            self.assertEqual(expected, actual)
    
    def test_file_info_absolute_paths(self):
        """Test that all FileInfo objects have absolute paths."""
        discovery = FileDiscovery(self.temp_dir)
        
        mock_results = {self.files['autowire']: (TEST_CONTENT_LITERAL, True, False)}
        
        with patch.object(discovery.scanner, 'scan', return_value=mock_results):
            actual = discovery.get_annotated_files()
            
            expected_files = {
                self.files['autowire']: FileInfo(self.files['autowire'], "content", True, False)
            }
            expected = ProjectFileCache(
                root_path=self.temp_dir,
                all_files=expected_files,
                autowire_files=[self.files['autowire']],
                provider_files=[]
            )
            
            self.assertEqual(expected, actual)
    
    def test_annotation_flags_consistency(self):
        """Test that annotation flags are consistent between FileInfo and lists."""
        discovery = FileDiscovery(self.temp_dir)
        
        mock_results = {
            self.files['autowire']: ("content", True, False),
            self.files['provider']: ("content", False, True),
            self.files['both']: ("content", True, True),
            self.files['plain']: ("content", False, False)
        }
        
        with patch.object(discovery.scanner, 'scan', return_value=mock_results):
            actual = discovery.get_annotated_files()
            
            expected_files = {
                self.files['autowire']: FileInfo(self.files['autowire'], "content", True, False),
                self.files['provider']: FileInfo(self.files['provider'], "content", False, True),
                self.files['both']: FileInfo(self.files['both'], "content", True, True),
                self.files['plain']: FileInfo(self.files['plain'], "content", False, False)
            }
            expected = ProjectFileCache(
                root_path=self.temp_dir,
                all_files=expected_files,
                autowire_files=[self.files['autowire'], self.files['both']],
                provider_files=[self.files['provider'], self.files['both']]
            )
            
            self.assertEqual(expected, actual)
    
    def test_multiple_files_discovery(self):
        """Test FileDiscovery with multiple files containing various annotations."""
        discovery = FileDiscovery(self.temp_dir)
        
        multi_files = create_multi_test_files(self.temp_dir)
        
        # Build mock results
        mock_results = build_mock_results_dict(multi_files, EXPECTED_MULTI_FILE_RESULTS)
        
        with patch.object(discovery.scanner, 'scan', return_value=mock_results):
            actual = discovery.get_annotated_files()
            
            # Build expected cache
            expected_files = {
                multi_files[key]: FileInfo(
                    multi_files[key], 
                    MULTI_FILE_CONTENTS[key], 
                    EXPECTED_MULTI_FILE_RESULTS[key][1],  # has_autowire
                    EXPECTED_MULTI_FILE_RESULTS[key][2]   # has_provider
                ) for key in MULTI_FILE_NAMES
            }
            
            expected_autowire = [multi_files[key] for key in MULTI_FILE_AUTOWIRE_KEYS]
            expected_provider = [multi_files[key] for key in MULTI_FILE_PROVIDER_KEYS]
            
            expected = ProjectFileCache(
                root_path=self.temp_dir,
                all_files=expected_files,
                autowire_files=expected_autowire,
                provider_files=expected_provider
            )
            
            self.assertEqual(expected, actual)