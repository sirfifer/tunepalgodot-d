extends VBoxContainer

var stuff = null
var current_tune_type = ["all"]
var buttons = []
var labels = []
var data_loaded = false
var record_control = null
var waiting_for_data = false
var button_to_tune_index = {}  # Maps button index to tune data index in 'stuff'

func _ready():
	buttons = get_children()
	# Initialize labels array and connect button signals
	for i in range(buttons.size()):
		var button = buttons[i]
		labels.append(button.get_children())
		button.visible = false
		# Connect the pressed signal to handle button clicks
		button.pressed.connect(_on_song_button_pressed.bind(i))
	# Start trying to load data
	_try_load_data()

func _on_song_button_pressed(button_index: int):
	# Navigate to ABCMenu when a song button is clicked
	if button_to_tune_index.has(button_index) and stuff != null:
		var tune_index = button_to_tune_index[button_index]
		if tune_index < stuff.size():
			var tune_data = stuff[tune_index]
			# Hide Keywords menu and show ABCMenu with tune data
			get_node("../../../../KeywordsMenu").visible = false
			var notation = tune_data.get("notation", "")
			var title = tune_data.get("title", "")
			get_node("../../../../ABCMenu/Control/ColorRect/ABC").text = notation
			get_node("../../../../ABCMenu/Control/ColorRect/Title").text = title
			get_node("../../../../ABCMenu").visible = true

func _try_load_data():
	if waiting_for_data:
		return
	waiting_for_data = true
	# Try to get the record control and its data
	_attempt_data_fetch()

func _attempt_data_fetch():
	record_control = get_node_or_null("../../../../RecordMenu/Control")
	if record_control:
		var existing_data = record_control.get("query_result")
		if existing_data != null and existing_data.size() > 0:
			print("Keywords found database with ", existing_data.size(), " tunes")
			stuff = existing_data
			data_loaded = true
			waiting_for_data = false
			_show_initial_tunes()
			return
	# Data not ready yet, retry
	await get_tree().create_timer(1.0).timeout
	_attempt_data_fetch()

func _refresh_data():
	# Get fresh reference to the query result
	if record_control == null:
		record_control = get_node_or_null("../../../../RecordMenu/Control")
	if record_control:
		stuff = record_control.get("query_result")
		if stuff != null and stuff.size() > 0:
			data_loaded = true

func _show_initial_tunes():
	# Show first 50 tunes alphabetically when no search is active
	if stuff == null or stuff.size() == 0:
		return

	# Clear button-to-tune mapping and hide all buttons first
	button_to_tune_index.clear()
	for i in range(buttons.size()):
		buttons[i].visible = false
		if labels[i].size() > 0:
			labels[i][0].visible = false

	# Show first 50 tunes
	var count = 0
	for i in range(stuff.size()):
		if count >= 50 or count >= buttons.size():
			break

		# Apply time signature filter
		if current_tune_type != ["all"]:
			var matches_filter = false
			for time_sig in current_tune_type:
				if stuff[i]["time_sig"] == time_sig:
					matches_filter = true
					break
			if not matches_filter:
				continue

		var button = buttons[count]
		button.set_text("  " + str(stuff[i]["title"]))
		button_to_tune_index[count] = i  # Map button index to tune data index

		var info_string = ""
		if stuff[i]["shortName"] != null:
			info_string = str(stuff[i]["shortName"])
		if stuff[i]["tune_type"] != null:
			info_string = info_string + " | " + str(stuff[i]["tune_type"])
		if stuff[i]["key_sig"] != null:
			info_string = info_string + " | " + str(stuff[i]["key_sig"])

		if labels[count].size() > 0:
			labels[count][0].set_text(info_string)
			labels[count][0].visible = true

		button.visible = true
		count += 1

func _on_search_bar_text_submitted(new_text):
	# If empty search, show initial tunes
	if new_text.strip_edges() == "":
		_show_initial_tunes()
		return

	# Ensure data is loaded
	if stuff == null or stuff.size() == 0:
		_refresh_data()
		if stuff == null or stuff.size() == 0:
			return

	# Clear button-to-tune mapping and hide all buttons first
	button_to_tune_index.clear()
	for i in range(buttons.size()):
		buttons[i].visible = false
		if i < labels.size() and labels[i].size() > 0:
			labels[i][0].visible = false

	var regex = RegEx.new()
	regex.compile("\\b\\w+\\b")
	var strings = regex.search_all(new_text)
	var search_terms = []
	for i in strings:
		search_terms.append(i.get_string().to_lower())

	var j = 0
	for i in range(0, stuff.size()):
		if j >= buttons.size():
			break

		var button = buttons[j]
		var check = false

		for term in search_terms:
			if current_tune_type != ["all"]:
				for time_sig in current_tune_type:
					if stuff[i]["title"].to_lower().contains(term) and time_sig == stuff[i]["time_sig"]:
						check = true
						break
			elif stuff[i]["title"].to_lower().contains(term):
				check = true
				break

		if check:
			button.set_text("  " + str(stuff[i]["title"]))
			button_to_tune_index[j] = i  # Map button index to tune data index
			var info_string = ""
			if stuff[i]["shortName"] != null:
				info_string = str(stuff[i]["shortName"])
			if stuff[i]["tune_type"] != null:
				info_string = info_string + " | " + str(stuff[i]["tune_type"])
			if stuff[i]["key_sig"] != null:
				info_string = info_string + " | " + str(stuff[i]["key_sig"])

			if j < labels.size() and labels[j].size() > 0:
				labels[j][0].set_text(info_string)
				labels[j][0].visible = true

			button.visible = true
			j += 1

		if j == 50:
			break
func _on_all_toggled(button_pressed):
	current_tune_type = ["all"]
	if button_pressed and data_loaded:
		_show_initial_tunes()

func _on_reels_hornpipes_toggled(button_pressed):
	current_tune_type = ["C", "C|", "4/4", "2/2", "4/2"]
	if button_pressed and data_loaded:
		_show_initial_tunes()

func _on_jigs_slides_etc_toggled(button_pressed):
	current_tune_type = ["12/8", "6/8"]
	if button_pressed and data_loaded:
		_show_initial_tunes()

func _on_slip_jigs_hop_jigs_toggled(button_pressed):
	current_tune_type = ["9/8"]
	if button_pressed and data_loaded:
		_show_initial_tunes()

func _on_waltzes_mazurkas_toggled(button_pressed):
	current_tune_type = ["3/4"]
	if button_pressed and data_loaded:
		_show_initial_tunes()

func _on_unsusual_jigs_toggled(button_pressed):
	current_tune_type = ["3/8"]
	if button_pressed and data_loaded:
		_show_initial_tunes()

func _on_unusual_english_hornpipes_toggled(button_pressed):
	current_tune_type = ["3/2", "6/4"]
	if button_pressed and data_loaded:
		_show_initial_tunes()
