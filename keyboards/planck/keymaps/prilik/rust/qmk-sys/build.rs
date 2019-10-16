use std::env;
use std::path::PathBuf;

fn main() {
    // Tell cargo to invalidate the built crate whenever the wrapper changes
    println!("cargo:rerun-if-changed=wrapper.h");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    let cur_path = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());

    // TODO: use environment variable!
    let qmk_base = "../../../../../../";

    let clang_flags = std::fs::read_to_string(format!(
        "{}.build/obj_planck_ez_prilik/preprocessor.txt",
        qmk_base
    ))
    .expect("unable to read preprocessor.txt flags from qmk");

    let bindings = bindgen::Builder::default()
        .header(cur_path.join("wrapper.h").to_str().unwrap())
        .use_core()
        .layout_tests(false)
        .ctypes_prefix("ctypes")
        .default_enum_style(bindgen::EnumVariation::ModuleConsts)
        .blacklist_item("FP_INFINITE")
        .blacklist_item("FP_NAN")
        .blacklist_item("FP_NORMAL")
        .blacklist_item("FP_SUBNORMAL")
        .blacklist_item("FP_ZERO")
        .blacklist_function("__.*")
        .clang_arg(format!("-working-directory={}", qmk_base))
        .clang_args(
            (String::from(" ") + &clang_flags)
                .trim()
                .split(" -")
                .skip(1)
                .map(|s| s.replace("include ", "include"))
                .map(|s| format!("-{}", s)),
        )
        .clang_arg("-Wno-invalid-pp-token")
        .generate()
        .expect("Unable to generate bindings");

    // Write the bindings to the $OUT_DIR/bindings.rs file.
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}
