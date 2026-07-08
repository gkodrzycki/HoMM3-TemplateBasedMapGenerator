const express = require("express");
const cors = require("cors");
const { execFile } = require("child_process");
const path = require("path");
const fs = require("fs");

const app = express();
const PORT = process.env.PORT || 3000;

// ========================
// PATHS
// ========================
const BASE_DIR = __dirname;

const GENERATOR_PATH = path.join(BASE_DIR, "../../Generator");
const OUTPUT_DIR = path.join(BASE_DIR, "output");
const FRONTEND_DIR = path.join(BASE_DIR, "../frontend");

// ========================
// MIDDLEWARE
// ========================
app.use(cors());
app.use(express.json());

// serve frontend
app.use(express.static(FRONTEND_DIR));

if (!fs.existsSync(OUTPUT_DIR)) {
    fs.mkdirSync(OUTPUT_DIR, { recursive: true });
}

// ========================
// CORE GENERATION LOGIC
// ========================
function runGenerator(seed, res) {
    const fileName = `map_${seed}.h3m`;
    const outputPath = path.join(OUTPUT_DIR, fileName);

    // NEW: CACHE CHECK
    if (fs.existsSync(outputPath)) {
        console.log(`✨ Map for seed ${seed} already exists. Serving cached file.`);
        return res.json({
            ok: true,
            seed,
            cached: true,
            file: `/download/${fileName}`
        });
    }

    console.log("====================================");
    console.log("Generating map");
    console.log("Seed:", seed);
    console.log("Output:", outputPath);
    console.log("Generator:", GENERATOR_PATH);
    console.log("====================================");

    // sanity check
    if (!fs.existsSync(GENERATOR_PATH)) {
        return res.status(500).json({
            ok: false,
            error: "Generator binary not found"
        });
    }

    // 1. Create the safety timeout wrapper variable
    let timeoutId;

    const child = execFile(
        GENERATOR_PATH,
        ["--seed", String(seed), "--location", outputPath],
        { maxBuffer: 1024 * 1024 * 50 },
        (err, stdout, stderr) => {

            // 2. Clear the timeout immediately when the process finishes!
            if (timeoutId) clearTimeout(timeoutId);

            if (err) {
                console.error("❌ Generator error:");
                console.error(stderr || err.message);

                // Check if the error was caused by our manual timeout kill
                if (err.signal === 'SIGKILL') {
                    return res.status(504).json({
                        ok: false,
                        error: "Generation timed out"
                    });
                }

                return res.status(500).json({
                    ok: false,
                    error: stderr?.toString() || err.message
                });
            }

            console.log(stdout);

            // verify output file exists
            if (!fs.existsSync(outputPath)) {
                return res.status(500).json({
                    ok: false,
                    error: "Map file was not created"
                });
            }

            res.json({
                ok: true,
                seed,
                cached: false,
                file: `/download/${fileName}`
            });
        }
    );

    // 3. Assign the active timer to the variable
    timeoutId = setTimeout(() => {
        console.log("⛔ Killing generator (timeout)");
        child.kill("SIGKILL");
    }, 120000); // 2 minutes
}

// ========================
// ROUTES
// ========================

// UI
app.get("/", (req, res) => {
    res.sendFile(path.join(FRONTEND_DIR, "index.html"));
});

app.get("/generate", (req, res) => {
    const seed = req.query.seed
        ? Number(req.query.seed)
        : Math.floor(Math.random() * 1e9);

    runGenerator(seed, res);
});

app.post("/generate", (req, res) => {
    const seed = req.body.seed
        ? Number(req.body.seed)
        : Math.floor(Math.random() * 1e9);

    runGenerator(seed, res);
});

app.get("/download/:file", (req, res) => {
    const safeFileName = path.basename(req.params.file);

    if (!safeFileName.endsWith('.h3m')) {
        return res.status(403).send("Forbidden: Invalid file extension.");
    }

    const filePath = path.join(OUTPUT_DIR, safeFileName);

    const absoluteOutputDir = path.resolve(OUTPUT_DIR);
    const absoluteFilePath = path.resolve(filePath);

    if (!absoluteFilePath.startsWith(absoluteOutputDir)) {
        return res.status(403).send("Forbidden: Path boundary violation.");
    }

    if (!fs.existsSync(absoluteFilePath)) {
        return res.status(404).send("File not found");
    }

    res.download(absoluteFilePath);
});

// ========================
// START SERVER
// ========================
app.listen(PORT, () => {
    console.log(`🚀 Server running on port ${PORT}`);
});
