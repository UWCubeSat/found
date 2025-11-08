"""Tests for annotation_scanner module."""

import unittest
import tempfile
import os
from unittest.mock import patch, MagicMock
from src.discovery.annotation_scanner import AnnotationScanner
from test.common.constants.discovery_test_constants import (
    TEST_AUTOWIRE_FILE, TEST_PROVIDER_FILE, TEST_BOTH_FILE, TEST_NONE_FILE, TEST_TXT_FILE,
    AUTOWIRE_CONTENT, PROVIDER_CONTENT, BOTH_CONTENT, NONE_CONTENT, TXT_CONTENT,
    WHITESPACE_CONTENT, TEST_ROOT_PATH, MOCK_GREP_PATH,
    MULTI_FILE_NAMES, MULTI_FILE_CONTENTS, EXPECTED_MULTI_FILE_RESULTS,
    NONE_FALSE_FALSE, WHITESPACE_FILENAME, AUTOWIRE_PATTERN_LITERAL, GREP_PATH_LITERAL,
    MOCK_GREP_RETURN, MOCK_SLOW_RETURN, create_temp_directory, cleanup_temp_directory,
    create_test_file, create_standard_test_files, create_multi_test_files
)


class TestAnnotationScanner(unittest.TestCase):
    """Test cases for AnnotationScanner class."""
    
    def _create_scanner(self, paths=None):
        """Helper method to create AnnotationScanner."""
        return AnnotationScanner(paths or [self.temp_dir])
    
    def setUp(self):
        """Set up test fixtures before each test method."""
        self.temp_dir = create_temp_directory()
        self.files = create_standard_test_files(self.temp_dir)
    
    def tearDown(self):
        """Clean up after each test method."""
        cleanup_temp_directory(self.temp_dir)
    
    def test_scan_file_with_autowire(self):
        """Test _scan_file method with AUTOWIRE annotation."""
        scanner = AnnotationScanner([self.temp_dir])
        
        result = scanner._scan_file(self.files['autowire'])
        
        self.assertEqual((AUTOWIRE_CONTENT, True, False), result)
    
    def test_scan_file_with_provider(self):
        """Test _scan_file method with PROVIDER annotation."""
        scanner = AnnotationScanner([self.temp_dir])
        
        result = scanner._scan_file(self.files['provider'])
        
        self.assertEqual((PROVIDER_CONTENT, False, True), result)
    
    def test_scan_file_with_both(self):
        """Test _scan_file method with both annotations."""
        scanner = AnnotationScanner([self.temp_dir])
        
        result = scanner._scan_file(self.files['both'])
        
        self.assertEqual((BOTH_CONTENT, True, True), result)
    
    def test_scan_file_with_none(self):
        """Test _scan_file method with no annotations."""
        scanner = AnnotationScanner([self.temp_dir])
        
        result = scanner._scan_file(self.files['none'])
        
        self.assertEqual((NONE_CONTENT, False, False), result)
    
    def test_scan_file_non_cpp(self):
        """Test _scan_file method with non-C++ file."""
        scanner = AnnotationScanner([self.temp_dir])
        
        result = scanner._scan_file(self.files['txt'])
        
        self.assertEqual(NONE_FALSE_FALSE, result)
    
    def test_scan_file_nonexistent(self):
        """Test _scan_file method with nonexistent file."""
        scanner = AnnotationScanner(["/test"])
        
        result = scanner._scan_file("/nonexistent/file.cpp")
        
        self.assertEqual(NONE_FALSE_FALSE, result)
    
    def test_slow_scan(self):
        """Test _slow_scan method."""
        scanner = AnnotationScanner([self.temp_dir])
        
        results = scanner._slow_scan()
        
        expected = {
            self.files['autowire']: (AUTOWIRE_CONTENT, True, False),
            self.files['provider']: (PROVIDER_CONTENT, False, True),
            self.files['both']: (BOTH_CONTENT, True, True),
            self.files['none']: (NONE_CONTENT, False, False)
        }
        
        self.assertDictEqual(expected, results)
    
    @patch('subprocess.run')
    def test_grep_for_annotation(self, mock_run):
        """Test _grep_for_annotation method."""
        scanner = AnnotationScanner([self.temp_dir])
        
        # Mock grep output
        mock_run.return_value.stdout = f"{self.files['autowire']}\n{self.files['both']}\n"
        
        result = scanner._grep_for_annotation(AUTOWIRE_PATTERN_LITERAL)
        expected = [os.path.abspath(self.files['autowire']), os.path.abspath(self.files['both'])]
        
        self.assertCountEqual(expected, result)
    
    @patch('subprocess.run')
    def test_grep_scan(self, mock_run):
        """Test _grep_scan method."""
        scanner = AnnotationScanner([self.temp_dir])
        
        # Mock grep outputs
        mock_run.side_effect = [
            MagicMock(stdout=f"{self.files['autowire']}\n{self.files['both']}\n"),  # AUTOWIRE files
            MagicMock(stdout=f"{self.files['provider']}\n{self.files['both']}\n")   # PROVIDER files
        ]
        
        results = scanner._grep_scan()
        
        expected = {
            os.path.abspath(self.files['autowire']): (None, True, False),
            os.path.abspath(self.files['provider']): (None, False, True),
            os.path.abspath(self.files['both']): (None, True, True)
        }
        
        self.assertDictEqual(expected, results)
    
    def test_scan_with_grep_available(self):
        """Test scan method when grep is available."""
        with patch('shutil.which', return_value=GREP_PATH_LITERAL):
            scanner = AnnotationScanner([self.temp_dir])
            
            with patch.object(scanner, '_grep_scan') as mock_grep:
                mock_grep.return_value = MOCK_GREP_RETURN
                
                result = scanner.scan()
                expected = MOCK_GREP_RETURN
                
                mock_grep.assert_called_once()
                self.assertDictEqual(expected, result)
    
    def test_scan_without_grep_available(self):
        """Test scan method when grep is not available."""
        with patch('shutil.which', return_value=None):
            scanner = AnnotationScanner([self.temp_dir])
            
            with patch.object(scanner, '_slow_scan') as mock_slow:
                mock_slow.return_value = MOCK_SLOW_RETURN
                
                result = scanner.scan()
                expected = MOCK_SLOW_RETURN
                
                mock_slow.assert_called_once()
                self.assertDictEqual(expected, result)
    
    def test_whitespace_in_annotations(self):
        """Test that annotations with whitespace are detected."""
        # Create file with whitespace around annotations
        whitespace_file = create_test_file(self.temp_dir, WHITESPACE_FILENAME, WHITESPACE_CONTENT)
        
        scanner = AnnotationScanner([self.temp_dir])
        result = scanner._scan_file(whitespace_file)
        
        self.assertTupleEqual((WHITESPACE_CONTENT, True, True), result)
    
    def test_multiple_files_with_annotations(self):
        """Test scanning multiple files with various annotation combinations."""
        # Create separate directory for this test
        multi_dir = create_temp_directory()
        scanner = AnnotationScanner([multi_dir])
        
        try:
            multi_files = create_multi_test_files(multi_dir)
            
            results = scanner._slow_scan()
            
            expected = {
                multi_files[key]: EXPECTED_MULTI_FILE_RESULTS[key] for key in MULTI_FILE_NAMES
            }
            
            self.assertDictEqual(expected, results)
        finally:
            cleanup_temp_directory(multi_dir)