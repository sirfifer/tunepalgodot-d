extends Control

# Debug Overlay for Experimental Detection System
# Shows real-time pitch detection info and allows parameter tuning

# References to experimental detector
var tunepal_exp = null
var is_available = false

# State
var recent_notes: Array = []
var max_recent_notes = 20
var current_status = "Idle"
var last_pitch = -1.0
var last_confidence = 0.0
var last_midi = -1

# UI References (set in _ready)
@onready var status_label = $Panel/VBox/StatusRow/StatusValue
@onready var pitch_label = $Panel/VBox/PitchRow/PitchValue
@onready var confidence_bar = $Panel/VBox/ConfidenceRow/ConfidenceBar
@onready var confidence_label = $Panel/VBox/ConfidenceRow/ConfidenceValue
@onready var midi_label = $Panel/VBox/MidiRow/MidiValue
@onready var yin_slider = $Panel/VBox/YinRow/YinSlider
@onready var yin_value_label = $Panel/VBox/YinRow/YinValue
@onready var min_freq_slider = $Panel/VBox/MinFreqRow/MinFreqSlider
@onready var min_freq_value_label = $Panel/VBox/MinFreqRow/MinFreqValue
@onready var max_freq_slider = $Panel/VBox/MaxFreqRow/MaxFreqSlider
@onready var max_freq_value_label = $Panel/VBox/MaxFreqRow/MaxFreqValue
@onready var notes_label = $Panel/VBox/NotesRow/NotesValue
@onready var close_button = $Panel/VBox/Header/CloseButton

func _ready():
	visible = false

	# Initialize experimental detector if available
	if ClassDB.class_exists("TunepalExperimental"):
		tunepal_exp = ClassDB.instantiate("TunepalExperimental")
		is_available = true
		print("Debug Overlay: TunepalExperimental loaded - ", tunepal_exp.get_version())

		# Set initial slider values from detector
		if yin_slider:
			yin_slider.value = tunepal_exp.get_yin_threshold()
			_update_yin_label()
	else:
		print("Debug Overlay: TunepalExperimental not available")

	# Connect close button
	if close_button:
		close_button.pressed.connect(_on_close_pressed)

	# Connect sliders
	if yin_slider:
		yin_slider.value_changed.connect(_on_yin_slider_changed)
	if min_freq_slider:
		min_freq_slider.value_changed.connect(_on_min_freq_changed)
	if max_freq_slider:
		max_freq_slider.value_changed.connect(_on_max_freq_changed)

func _process(_delta):
	if visible and is_available:
		_update_display()

func toggle():
	visible = !visible
	if visible:
		_update_display()

func show_overlay():
	visible = true
	_update_display()

func hide_overlay():
	visible = false

func _on_close_pressed():
	hide_overlay()

# Called from record.gd when a pitch is detected
func update_pitch(frequency: float, confidence: float):
	last_pitch = frequency
	last_confidence = confidence

	if frequency > 0:
		last_midi = _frequency_to_midi(frequency)
		var note_name = _midi_to_note_name(last_midi)

		# Add to recent notes (avoid duplicates in a row)
		if recent_notes.size() == 0 or recent_notes[-1] != note_name:
			recent_notes.append(note_name)
			if recent_notes.size() > max_recent_notes:
				recent_notes.pop_front()
	else:
		last_midi = -1

func set_status(status: String):
	current_status = status

func _update_display():
	if not is_available:
		if status_label:
			status_label.text = "Not Available"
		return

	# Status
	if status_label:
		status_label.text = current_status

	# Pitch
	if pitch_label:
		if last_pitch > 0:
			var note_name = _midi_to_note_name(last_midi)
			var octave = _midi_to_octave(last_midi)
			pitch_label.text = "%.1f Hz (%s%d)" % [last_pitch, note_name, octave]
		else:
			pitch_label.text = "-- Hz"

	# Confidence
	if confidence_bar:
		confidence_bar.value = last_confidence
	if confidence_label:
		confidence_label.text = "%.2f" % last_confidence

	# MIDI
	if midi_label:
		if last_midi > 0:
			midi_label.text = str(last_midi)
		else:
			midi_label.text = "--"

	# Recent notes
	if notes_label:
		if recent_notes.size() > 0:
			notes_label.text = " ".join(recent_notes.slice(-10))  # Last 10
		else:
			notes_label.text = "--"

func _on_yin_slider_changed(value: float):
	if tunepal_exp:
		tunepal_exp.set_yin_threshold(value)
	_update_yin_label()

func _update_yin_label():
	if yin_value_label and yin_slider:
		yin_value_label.text = "%.2f" % yin_slider.value

func _on_min_freq_changed(value: float):
	if tunepal_exp:
		tunepal_exp.set_pitch_config(44100.0, 2048, value, max_freq_slider.value if max_freq_slider else 2000.0)
	if min_freq_value_label:
		min_freq_value_label.text = "%d Hz" % int(value)

func _on_max_freq_changed(value: float):
	if tunepal_exp:
		tunepal_exp.set_pitch_config(44100.0, 2048, min_freq_slider.value if min_freq_slider else 80.0, value)
	if max_freq_value_label:
		max_freq_value_label.text = "%d Hz" % int(value)

func clear_notes():
	recent_notes.clear()
	if notes_label:
		notes_label.text = "--"

# Helper functions
func _frequency_to_midi(frequency: float) -> int:
	if frequency <= 0:
		return -1
	return int(round(69.0 + 12.0 * log(frequency / 440.0) / log(2.0)))

func _midi_to_note_name(midi: int) -> String:
	if midi < 0:
		return ""
	var notes = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]
	return notes[midi % 12]

func _midi_to_octave(midi: int) -> int:
	if midi < 0:
		return -1
	return (midi / 12) - 1
