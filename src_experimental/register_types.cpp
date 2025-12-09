/**
 * GDExtension registration for TunepalExperimental
 *
 * This registers the experimental algorithms as a SEPARATE extension
 * from Bryan's original Tunepal library.
 */

#include "register_types.h"
#include "tunepal_experimental.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void initialize_tunepal_experimental_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    ClassDB::register_class<TunepalExperimental>();
}

void uninitialize_tunepal_experimental_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
// Initialization
GDExtensionBool GDE_EXPORT tunepal_experimental_library_init(
    GDExtensionInterfaceGetProcAddress p_get_proc_address,
    const GDExtensionClassLibraryPtr p_library,
    GDExtensionInitialization *r_initialization
) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_tunepal_experimental_module);
    init_obj.register_terminator(uninitialize_tunepal_experimental_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}
