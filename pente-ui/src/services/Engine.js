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
  const wasmMemory = new WebAssembly.Memory({ initial: 256, maximum: 512 });
  const wasmTable = new WebAssembly.Table({ initial: 0, element: "anyfunc" });

  const wasmImports = {
    __assert_fail: () => {},
    __cxa_throw: () => {},
    _abort_js: () => {},
    _emscripten_memcpy_js: () => {},
    emscripten_resize_heap: () => {},
    fd_close: () => {},
    fd_seek: () => {},
    fd_write: () => {},
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

  console.log(wasmModule.instance.exports);

  const getNextMove = wasmModule.instance.exports.getNextMove;
  const malloc = wasmModule.instance.exports.malloc;
  const free = wasmModule.instance.exports.free;
  const HEAP32 = new Int32Array(wasmModule.instance.exports.memory.buffer);

  return {
    getNextMove,
    malloc,
    free,
    HEAP32,
  };
};
