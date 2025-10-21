# Burma Automaton - Quick Start Guide

## Fresh Install (One Command)

If you're setting up from scratch:

```bash
cd /home/leo/Burma_Automaton
./setup-all.sh
```

This will:
1. ✅ Install **ALL** dependencies automatically (Qt6, cmake, gcc, python3, etc.)
2. ✅ Build BitNet.cpp with gcc (no clang needed!)
3. ✅ Download BitNet model (~1.2GB)
4. ✅ Build Burma Automaton
5. ✅ Verify everything works

**Time**: 10-20 minutes (depending on internet speed)

## Already Built? Just Run It!

If you already have everything set up (like now):

```bash
cd /home/leo/Burma_Automaton
./build/BurmaAutomaton
```

That's it! The application will automatically find BitNet.cpp and show `Ready: Yes`.

## What You Have Now

```
✓ BitNet.cpp built with gcc
  ~/BitNet/build/bin/llama-cli

✓ Model downloaded (1.2GB)
  ~/BitNet/models/BitNet-b1.58-2B-4T/ggml-model-i2_s.gguf

✓ Burma Automaton built
  ./build/BurmaAutomaton
```

## How It Works

```
User types prompt in UI
    ↓
BitNetClient (C++)
    ↓
QProcess launches llama-cli
    ↓
BitNet.cpp runs inference (1.58-bit, CPU)
    ↓
JSON world plan generated
    ↓
3D scene rendered
```

**No HTTP server, no Python runtime needed!**

## Why That `setup_env.py` Error Doesn't Matter

The error you saw:
```
ERROR: Command '['cmake', '-B', 'build', '-DCMAKE_C_COMPILER=clang', ...]
```

**Reason**: `setup_env.py` is hardcoded to use `clang`, but:
- ✅ You have `gcc` (which works perfectly)
- ✅ We already built BitNet.cpp with gcc
- ✅ The model is already downloaded
- ✅ Everything is ready to run

See [BITNET_SETUP_EXPLAINED.md](BITNET_SETUP_EXPLAINED.md) for details.

## Testing

### Test 1: BitNet.cpp directly
```bash
~/BitNet/build/bin/llama-cli \
    -m ~/BitNet/models/BitNet-b1.58-2B-4T/ggml-model-i2_s.gguf \
    -p "Hello" \
    -n 20
```
Expected: Model loads and generates text.

### Test 2: Burma Automaton
```bash
./build/BurmaAutomaton
```
Expected: Event Log shows "BitNet.cpp Client Initialized" and "Ready: Yes"

### Test 3: Submit a prompt
In the UI, type: **"Create a simple box"**
Expected: Processing runs, world plan generated, 3D scene rendered.

## Files Summary

| File | Purpose |
|------|---------|
| `setup-all.sh` | One-command setup (installs everything) |
| `build.sh` | Just build Burma Automaton |
| `RUN.md` | Detailed running instructions |
| `BITNET_SETUP_EXPLAINED.md` | Why the clang error doesn't matter |
| `QUICKSTART.md` | This file |

## Need Help?

- **Setup issues**: Run `./setup-all.sh` again
- **Build issues**: Run `./build.sh --clean`
- **BitNet not found**: Check paths in Event Log
- **Other issues**: See [RUN.md](RUN.md)

---

**You're all set!** Just run `./build/BurmaAutomaton` and enjoy! 🚀
