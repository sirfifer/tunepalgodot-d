extends Node
## Test suite for Tunepal extension
## Run this scene to verify the edit distance algorithm works correctly

var tunepal: Tunepal
var tests_passed := 0
var tests_failed := 0

func _ready():
	print("=" .repeat(60))
	print("Tunepal Extension Test Suite")
	print("=" .repeat(60))

	tunepal = Tunepal.new()

	# Run all tests
	test_exact_match()
	test_single_insertion()
	test_single_deletion()
	test_single_substitution()
	test_substring_match()
	test_empty_strings()
	test_real_tune_patterns()

	# Print summary
	print("")
	print("=" .repeat(60))
	print("RESULTS: %d passed, %d failed" % [tests_passed, tests_failed])
	print("=" .repeat(60))

	if tests_failed > 0:
		print("SOME TESTS FAILED!")
	else:
		print("ALL TESTS PASSED!")

	# Clean up
	tunepal.free()

func assert_eq(actual, expected, test_name: String):
	if actual == expected:
		print("  PASS: %s" % test_name)
		tests_passed += 1
	else:
		print("  FAIL: %s (expected %s, got %s)" % [test_name, expected, actual])
		tests_failed += 1

func test_exact_match():
	print("\nTest: Exact Match")
	# When pattern is found exactly in text, edit distance should be 0
	var ed = tunepal.edSubstring("ABC", "XYZABCDEF", 0)
	assert_eq(ed, 0, "Exact substring match returns 0")

func test_single_insertion():
	print("\nTest: Single Insertion")
	# Pattern "AC" in text "ABC" - need 1 deletion to match
	var ed = tunepal.edSubstring("AC", "ABC", 0)
	assert_eq(ed, 0, "Pattern AC found in ABC with 0 edits")

func test_single_deletion():
	print("\nTest: Single Deletion")
	# Pattern "ABCD" in text "ABC" - pattern longer than text
	var ed = tunepal.edSubstring("ABD", "ABCD", 0)
	assert_eq(ed, 0, "ABD found as substring in ABCD")

func test_single_substitution():
	print("\nTest: Single Substitution")
	# Pattern "AXC" in text "ABC" - need 1 substitution
	var ed = tunepal.edSubstring("AXC", "ABC", 0)
	assert_eq(ed, 1, "AXC requires 1 substitution to match ABC")

func test_substring_match():
	print("\nTest: Substring Matching")
	# The algorithm finds the best substring match
	var ed = tunepal.edSubstring("DEF", "ABCDEFGHI", 0)
	assert_eq(ed, 0, "DEF found exactly in middle of text")

	ed = tunepal.edSubstring("DEFG", "ABCDEFGHI", 0)
	assert_eq(ed, 0, "DEFG found exactly in text")

func test_empty_strings():
	print("\nTest: Empty Strings")
	# Empty pattern should return 0
	var ed = tunepal.edSubstring("", "ABC", 0)
	assert_eq(ed, 0, "Empty pattern returns 0")

	ed = tunepal.edSubstring("ABC", "", 0)
	assert_eq(ed, 0, "Empty text returns 0")

func test_real_tune_patterns():
	print("\nTest: Real Tune Patterns (ABC notation style)")
	# Simulating tune matching with note sequences
	var tune_db = "GABCDEDCBAGABCDEDCBAGABCDEDCBA"

	# Exact phrase
	var ed = tunepal.edSubstring("GABCDE", tune_db, 0)
	assert_eq(ed, 0, "Exact phrase GABCDE found")

	# Phrase with one wrong note
	ed = tunepal.edSubstring("GAXCDE", tune_db, 0)
	assert_eq(ed, 1, "GAXCDE has 1 error (X instead of B)")

	# Short pattern
	ed = tunepal.edSubstring("CDE", tune_db, 0)
	assert_eq(ed, 0, "Short pattern CDE found")

	# The Kesh Jig style pattern
	var kesh_style = "GAGBAGEGDGAGBAGEGDG"
	ed = tunepal.edSubstring("GAGBAG", kesh_style, 0)
	assert_eq(ed, 0, "Kesh-style pattern matched")

# Called when run as autoload or standalone scene
func _enter_tree():
	if get_parent() == get_tree().root:
		# Running as main scene
		call_deferred("_ready")
