//! Rust adapter for the PolyTest C ABI.
//!
//! Default: `#![no_std]`. Enable feature `std` for host binaries.
//!
//! Link against the C harness (`polytest_core.c` / amalgam `polytest.c`),
//! register cases from C or via FFI, then call [`ffi::polytest_run_all`] or
//! [`std_support::run_from_env`].
//!
//! A `#[polytest::test]` proc-macro is planned later; it is not required for v1.
//!
//! # Example (host)
//!
//! See `examples/host_rust`.

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
        pub fn polytest_run_suite(suite: *const c_char) -> c_int;
        pub fn polytest_run_group(suite: *const c_char, group: *const c_char) -> c_int;
        pub fn polytest_run_from_env() -> c_int;
        pub fn polytest_fail(message: *const c_char, file: *const c_char, line: c_int);
        pub fn polytest_fail_at(file: *const c_char, line: c_int, message: *const c_char);
        pub fn polytest_set_param(index: usize, param: *const c_void);
        pub fn polytest_clear_param();
        pub fn polytest_param_index() -> usize;
        pub fn polytest_current_param() -> *const c_void;
        pub fn polytest_set_locks(
            lock: PolytestLockFn,
            unlock: PolytestLockFn,
            user: *mut c_void,
        );
    }
}

#[cfg(feature = "std")]
pub mod std_support {
    //! Host helpers when the `std` feature is enabled.
    use std::ffi::CString;

    use crate::ffi;

    /// Run all registered C cases and return the process-style exit code.
    pub fn run_all() -> i32 {
        unsafe { ffi::polytest_run_all() }
    }

    /// Run cases matching a tag.
    pub fn run_tag(tag: &str) -> i32 {
        let c = CString::new(tag).expect("tag contains NUL");
        unsafe { ffi::polytest_run_tag(c.as_ptr()) }
    }

    /// Run cases in a suite.
    pub fn run_suite(suite: &str) -> i32 {
        let c = CString::new(suite).expect("suite contains NUL");
        unsafe { ffi::polytest_run_suite(c.as_ptr()) }
    }

    /// Run cases in a suite/group.
    pub fn run_group(suite: &str, group: &str) -> i32 {
        let s = CString::new(suite).expect("suite contains NUL");
        let g = CString::new(group).expect("group contains NUL");
        unsafe { ffi::polytest_run_group(s.as_ptr(), g.as_ptr()) }
    }

    /// Honor `POLYTEST_TAG` / `POLYTEST_SUITE` / `POLYTEST_GROUP`.
    pub fn run_from_env() -> i32 {
        unsafe { ffi::polytest_run_from_env() }
    }
}
