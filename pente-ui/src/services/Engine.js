export const randomEngine = (squares) => {
  // Example AI: randomly picks an empty square
  const emptySquares = squares
    .map((square, index) => (square === null ? index : null))
    .filter((val) => val !== null);
  const randomMove =
    emptySquares[Math.floor(Math.random() * emptySquares.length)];
  return randomMove;
};

export const loadWasm = async () => {
  const wasmMemory = new WebAssembly.Memory({ initial: 1024, maximum: 65537 });
  const wasmTable = new WebAssembly.Table({ initial: 0, element: "anyfunc" });

  const wasmImports = {
    __assert_fail: () => console.log("__assert_fail"),
    __cxa_throw: () => console.log("__cxa_throw"),
    __syscall_dup3: () => console.log("__syscall_dup3"),
    __syscall_fcntl64: () => console.log("__syscall_fcntl64"),
    __syscall_ioctl: () => console.log("__syscall_ioctl"),
    __syscall_openat: () => console.log("__syscall_openat"),
    _abort_js: () => console.log("_abort_js"),
    _emscripten_memcpy_js: () => console.log("_emscripten_memcpy_js"),
    emscripten_console_log: (ptr) => {
      const memory = new Uint8Array(wasmModule.instance.exports.memory.buffer);
      let message = "";
      for (let i = ptr; memory[i] !== 0; i++) {
        message += String.fromCharCode(memory[i]);
      }
      console.log(message);
    },
    // emscripten_resize_heap: () => console.log("emscripten_resize_heap"),
    emscripten_get_heap_size: () => wasmModule.instance.exports.memory.buffer.byteLength,
    emscripten_resize_heap: (size) => {
      try {
        wasmModule.instance.exports.memory.grow(size / 65536);
        return 1;
      } catch (e) {
        console.log("Failed to resize heap:", e);
        return 0;
      }
    },
    environ_get: () => console.log("environ_get"),
    environ_sizes_get: () => console.log("environ_sizes_get"),
    fd_close: () => console.log("fd_close"),
    fd_read: () => console.log("fd_read"),
    fd_seek: () => console.log("fd_seek"),
    fd_write: () => console.log("fd_write"),
    strftime_l: () => console.log("strftime_l"),
    memory: wasmMemory,
    table: wasmTable,
  };

  const info = {
    env: wasmImports,
    wasi_snapshot_preview1: wasmImports,
  };

  const response = await fetch("/penteEngine.wasm");
  const buffer = await response.arrayBuffer();
  const wasmModule = await WebAssembly.instantiate(buffer, info);

  const getNextMove = wasmModule.instance.exports.getNextMove;
  const getBoard = wasmModule.instance.exports.getBoard;
  const malloc = wasmModule.instance.exports.malloc;
  const free = wasmModule.instance.exports.free;
  const HEAP32 = new Int32Array(wasmModule.instance.exports.memory.buffer);

  return {
    getNextMove,
    getBoard,
    malloc,
    free,
    HEAP32,
  };
};
