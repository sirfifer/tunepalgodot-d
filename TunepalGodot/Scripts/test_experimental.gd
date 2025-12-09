extends Node

# Quick test to verify both Tunepal and TunepalExperimental can be loaded

func _ready():
	print("=== Testing GDExtension Libraries ===")

	# Test original Tunepal
	if ClassDB.class_exists("Tunepal"):
		print("[OK] Tunepal class exists (Bryan's original)")
		var tunepal = ClassDB.instantiate("Tunepal")
		if tunepal:
			tunepal.say_hello()
			print("[OK] Tunepal instantiated successfully")
		else:
			print("[FAIL] Could not instantiate Tunepal")
	else:
		print("[SKIP] Tunepal class not available (expected on iOS/mobile)")

	# Test experimental TunepalExperimental
	if ClassDB.class_exists("TunepalExperimental"):
		print("[OK] TunepalExperimental class exists (pYIN + DTW)")
		var exp = ClassDB.instantiate("TunepalExperimental")
		if exp:
			exp.say_hello()
			print("[OK] TunepalExperimental version: ", exp.get_version())

			# Test DTW matching
			var sim = exp.dtw_similarity("CDEFG", "CDEFGAB")
			print("[OK] DTW similarity test: ", sim)

			# Test YIN threshold getter/setter
			print("[OK] YIN threshold: ", exp.get_yin_threshold())

			print("[OK] TunepalExperimental instantiated successfully")
		else:
			print("[FAIL] Could not instantiate TunepalExperimental")
	else:
		print("[FAIL] TunepalExperimental class not found!")

	print("=== Test Complete ===")
	get_tree().quit()
