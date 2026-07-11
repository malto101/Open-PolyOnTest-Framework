//! Rust adapter for the PolyTest C ABI.
//!
//! Default: `#![no_std]`. Enable feature `std` for host binaries.
//!
//! Link against the C harness (`polytest_core.c` / amalgam `polytest.c`),
//! register cases from C or via FFI, then call [`ffi::polytest_run_all`].
//!
//! A `#[polytest::test]` proc-macro is planned later; it is not required for v1.
//!
//! # Example (host)
//!
//! ```ignore
//! use polytest_rs::ffi::polytest_run_all;
//!
//! fn main() {
//!     let code = unsafe { polytest_run_all() };
//!     std::process::exit(code);
//! }
//! ```

#![cfg_attr(not(feature = "std"), no_std)]

/// Crate version string.
pub const POLYTEST_RS_VERSION: &str = env!("CARGO_PKG_VERSION");

/// FFI declarations matching `harness/include/polytest/polytest.h`.
pub mod ffi {
    use core::ffi::{c_char, c_int, c_void};

    pub type PolytestFn = Option<unsafe extern "C" fn()>;
    pub type PolytestWriteFn =
        Option<unsafe extern "C" fn(data: *const c_void, len: usize, user: *mut c_void)>;
    pub type PolytestLockFn = Option<unsafe extern "C" fn(user: *mut c_void)>;

    extern "C" {
        pub fn polytest_set_writer(fn_: PolytestWriteFn, user: *mut c_void);
        pub fn polytest_run_all() -> c_int;
        pub fn polytest_run_tag(tag: *const c_char) -> c_int;
        pub fn polytest_fail(message: *const c_char, file: *const c_char, line: c_int);
    }
}

#[cfg(feature = "std")]
pub mod std_support {
    //! Host helpers when the `std` feature is enabled.
    pub use crate::ffi::polytest_run_all;

    /// Run all registered C cases and return the process-style exit code.
    pub fn run_all() -> i32 {
        unsafe { polytest_run_all() }
    }
}
