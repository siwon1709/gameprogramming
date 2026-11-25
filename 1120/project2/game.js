// ==========================================
// 1. 기본 설정, 테마 팔레트 및 상수
// ==========================================
const canvas = document.getElementById('game');
const ctx = canvas.getContext('2d');
const holdCanvas = document.getElementById('holdCanvas');
const holdCtx = holdCanvas.getContext('2d');
const nextCanvas = document.getElementById('nextCanvas');
const nextCtx = nextCanvas.getContext('2d');

const COLS = 10;
const ROWS = 20;
const BLOCK = 30;

// 블록 ID: 1~7 일반 테트로미노, 8: 폭탄, 9: 단단한 돌(2타), 10: 손상된 돌(1타), 11: 와일드 무지개, 12: 가비지
const SHAPES = [
  [],
  [[1, 1, 1, 1]],
  [[2, 0, 0], [2, 2, 2]],
  [[0, 0, 3], [3, 3, 3]],
  [[4, 4], [4, 4]],
  [[0, 5, 5], [5, 5, 0]],
  [[0, 6, 0], [6, 6, 6]],
  [[7, 7, 0], [0, 7, 7]]
];

const THEMES = {
  neon: {
    accent: '#22d3ee',
    glow: 'rgba(34, 211, 238, 0.35)',
    colors: [null, '#22d3ee', '#60a5fa', '#fb923c', '#facc15', '#4ade80', '#c084fc', '#f87171', '#ef4444', '#64748b', '#94a3b8', '#ffffff', '#3f3f46']
  },
  cyber: {
    accent: '#f472b6',
    glow: 'rgba(244, 114, 182, 0.35)',
    colors: [null, '#38bdf8', '#818cf8', '#f472b6', '#fde047', '#34d399', '#e879f9', '#fb7185', '#dc2626', '#52525b', '#a1a1aa', '#ffffff', '#3f3f46']
  },
  matrix: {
    accent: '#4ade80',
    glow: 'rgba(74, 222, 128, 0.35)',
    colors: [null, '#4ade80', '#22c55e', '#86efac', '#bef264', '#10b981', '#2dd4bf', '#a3e635', '#ef4444', '#475569', '#94a3b8', '#ffffff', '#27272a']
  },
  retro: {
    accent: '#facc15',
    glow: 'rgba(250, 204, 21, 0.3)',
    colors: [null, '#00f0f0', '#0000f0', '#f0a000', '#f0f000', '#00f000', '#a000f0', '#f00000', '#ff3333', '#666666', '#999999', '#ffffff', '#444444']
  }
};

let currentTheme = 'neon';

// ==========================================
// 2. Web Audio API 신스 효과음 엔진 (16번)
// ==========================================
const AudioEngine = {
  ctx: null,
  muted: false,
  init() {
    if (!this.ctx) {
      this.ctx = new (window.AudioContext || window.webkitAudioContext)();
    }
    if (this.ctx.state === 'suspended') {
      this.ctx.resume();
    }
  },
  play(type) {
    if (this.muted) return;
    this.init();
    const now = this.ctx.currentTime;
    const osc = this.ctx.createOscillator();
    const gain = this.ctx.createGain();
    osc.connect(gain);
    gain.connect(this.ctx.destination);

    if (type === 'move') {
      osc.type = 'triangle';
      osc.frequency.setValueAtTime(220, now);
      gain.gain.setValueAtTime(0.05, now);
      gain.gain.exponentialRampToValueAtTime(0.001, now + 0.05);
      osc.start(now);
      osc.stop(now + 0.05);
    } else if (type === 'rotate') {
      osc.type = 'sine';
      osc.frequency.setValueAtTime(360, now);
      osc.frequency.exponentialRampToValueAtTime(520, now + 0.07);
      gain.gain.setValueAtTime(0.07, now);
      gain.gain.exponentialRampToValueAtTime(0.001, now + 0.07);
      osc.start(now);
      osc.stop(now + 0.07);
    } else if (type === 'drop') {
      osc.type = 'sawtooth';
      osc.frequency.setValueAtTime(160, now);
      osc.frequency.exponentialRampToValueAtTime(55, now + 0.12);
      gain.gain.setValueAtTime(0.12, now);
      gain.gain.exponentialRampToValueAtTime(0.001, now + 0.12);
      osc.start(now);
      osc.stop(now + 0.12);
    } else if (type === 'clear') {
      osc.type = 'square';
      osc.frequency.setValueAtTime(440, now);
      osc.frequency.setValueAtTime(660, now + 0.08);
      osc.frequency.setValueAtTime(880, now + 0.16);
      gain.gain.setValueAtTime(0.09, now);
      gain.gain.exponentialRampToValueAtTime(0.001, now + 0.28);
      osc.start(now);
      osc.stop(now + 0.28);
    } else if (type === 'bomb') {
      osc.type = 'sawtooth';
      osc.frequency.setValueAtTime(120, now);
      osc.frequency.exponentialRampToValueAtTime(30, now + 0.35);
      gain.gain.setValueAtTime(0.18, now);
      gain.gain.exponentialRampToValueAtTime(0.001, now + 0.35);
      osc.start(now);
      osc.stop(now + 0.35);
    } else if (type === 'levelup') {
      osc.type = 'triangle';
      [523, 659, 784, 1046].forEach((freq, i) => {
        osc.frequency.setValueAtTime(freq, now + i * 0.07);
      });
      gain.gain.setValueAtTime(0.1, now);
      gain.gain.exponentialRampToValueAtTime(0.001, now + 0.35);
      osc.start(now);
      osc.stop(now + 0.35);
    } else if (type === 'gameover') {
      osc.type = 'sawtooth';
      osc.frequency.setValueAtTime(300, now);
      osc.frequency.linearRampToValueAtTime(80, now + 0.5);
      gain.gain.setValueAtTime(0.12, now);
      gain.gain.exponentialRampToValueAtTime(0.001, now + 0.5);
      osc.start(now);
      osc.stop(now + 0.5);
    }
  }
};

// ==========================================
// 3. 영구 저장 통계 (localStorage - 21번)
// ==========================================
const Storage = {
  key: 'neon_tetris_ultimate_v1',
  data: {
    bestScore: 0,
    bestSprintMs: null,
    totalGames: 0,
    totalLines: 0
  },
  load() {
    try {
      const saved = JSON.parse(localStorage.getItem(this.key));
      if (saved) Object.assign(this.data, saved);
    } catch (e) {}
    this.updateUI();
  },
  save() {
    try {
      localStorage.setItem(this.key, JSON.stringify(this.data));
    } catch (e) {}
    this.updateUI();
  },
  updateUI() {
    document.getElementById('best-score').textContent = `BEST: ${this.data.bestScore.toLocaleString()}`;
    document.getElementById('lifetime-stats').textContent =
      `누적 플레이: ${this.data.totalGames}회 | 누적 줄: ${this.data.totalLines}줄`;
  }
};

// ==========================================
// 4. 게임 상태 변수
// ==========================================
function createBoard() {
  return Array.from({ length: ROWS }, () => Array(COLS).fill(0));
}

let board = createBoard();
let gameMode = 'marathon'; // marathon | sprint | ultra | garbage
let startLevel = 1;
let level = 1;
let score = 0;
let linesCleared = 0;
let combo = -1;
let maxCombo = 0;
let backToBack = false;
let tetrisCount = 0;
let skillGauge = 0; // 0 ~ 100
let slowTimer = 0; // ms
let garbageTimer = 0;

let dropCounter = 0;
let dropInterval = 700;
let lockDelayCounter = 0;
const LOCK_DELAY_LIMIT = 450;

let elapsedMs = 0;
let lastTime = 0;
let gameState = 'menu'; // menu | playing | paused | gameover

// 7-Bag 큐 & 홀드
let bag = [];
let nextQueue = [];
let holdMatrix = null;
let canHold = true;

let player = {
  pos: { x: 0, y: 0 },
  matrix: null
};

// 시각 이펙트 배열 (파티클, 팝업, 하드드롭 잔상, 라인 플래시, 화면 흔들림)
let particles = [];
let floatingTexts = [];
let dropTrails = [];
let flashRows = [];
let shakeTime = 0;
let shakeMagnitude = 0;

// ==========================================
// 5. 7-Bag & 특수 블록 생성 (4, 22, 23, 24번)
// ==========================================
function refillBag() {
  const pieces = [1, 2, 3, 4, 5, 6, 7];
  for (let i = pieces.length - 1; i > 0; i--) {
    const j = Math.floor(Math.random() * (i + 1));
    [pieces[i], pieces[j]] = [pieces[j], pieces[i]];
  }
  bag.push(...pieces);
}

function createPieceMatrix() {
  if (bag.length < 7) refillBag();
  const id = bag.shift();
  const matrix = SHAPES[id].map(row => [...row]);

  // 특수 블록 확률 부여 (폭탄 8, 돌 9, 와일드 11)
  const roll = Math.random();
  const cells = [];
  matrix.forEach((r, y) => r.forEach((v, x) => { if (v !== 0) cells.push({ x, y }); }));
  const target = cells[Math.floor(Math.random() * cells.length)];

  if (roll < 0.08) {
    matrix[target.y][target.x] = 8; // 💣 폭탄 블록
  } else if (roll < 0.14) {
    matrix[target.y][target.x] = 9; // 🪨 단단한 돌 블록 (2회 타격 파괴)
  } else if (roll < 0.19) {
    matrix[target.y][target.x] = 11; // 🌈 와일드 무지개 블록
  }
  return matrix;
}

function initNextQueue() {
  bag = [];
  nextQueue = [createPieceMatrix(), createPieceMatrix(), createPieceMatrix()];
}

function spawnPlayer(customMatrix = null) {
  if (customMatrix) {
    player.matrix = customMatrix.map(r => [...r]);
  } else {
    player.matrix = nextQueue.shift();
    nextQueue.push(createPieceMatrix());
    canHold = true;
  }
  player.pos.y = 0;
  player.pos.x = Math.floor((COLS - player.matrix[0].length) / 2);
  lockDelayCounter = 0;

  if (collide(board, player)) {
    endGame(false);
  }
  drawMiniPreviews();
  updateUI();
}

// ==========================================
// 6. 충돌, 월킥 회전, 고스트 위치 계산 (2, 5번)
// ==========================================
function collide(grid, piece) {
  const { matrix, pos } = piece;
  for (let y = 0; y < matrix.length; y++) {
    for (let x = 0; x < matrix[y].length; x++) {
      if (matrix[y][x] !== 0) {
        const nx = pos.x + x;
        const ny = pos.y + y;
        if (nx < 0 || nx >= COLS || ny >= ROWS) return true;
        if (ny >= 0 && grid[ny][nx] !== 0) return true;
      }
    }
  }
  return false;
}

function rotateMatrix(matrix) {
  const rows = matrix.length;
  const cols = matrix[0].length;
  return Array.from({ length: cols }, (_, c) =>
    Array.from({ length: rows }, (_, r) => matrix[rows - 1 - r][c])
  );
}

function tryRotate() {
  const prevMatrix = player.matrix;
  const prevX = player.pos.x;
  const prevY = player.pos.y;
  player.matrix = rotateMatrix(player.matrix);

  // 월킥 오프셋 테스트
  const kicks = [
    [0, 0], [-1, 0], [1, 0], [-2, 0], [2, 0], [0, -1], [-1, -1], [1, -1]
  ];
  for (const [kx, ky] of kicks) {
    player.pos.x = prevX + kx;
    player.pos.y = prevY + ky;
    if (!collide(board, player)) {
      lockDelayCounter = 0;
      AudioEngine.play('rotate');
      return;
    }
  }
  // 회전 불가 시 복구
  player.matrix = prevMatrix;
  player.pos.x = prevX;
  player.pos.y = prevY;
}

function getGhostY() {
  const ghost = { pos: { x: player.pos.x, y: player.pos.y }, matrix: player.matrix };
  while (!collide(board, ghost)) {
    ghost.pos.y++;
  }
  return ghost.pos.y - 1;
}

// ==========================================
// 7. 병합, 폭탄 폭발, 돌 블록, 줄 삭제 (9, 10, 22, 23, 24번)
// ==========================================
function triggerShake(duration, magnitude) {
  shakeTime = duration;
  shakeMagnitude = magnitude;
}

function spawnParticles(gridX, gridY, color, count = 10) {
  const cx = gridX * BLOCK + BLOCK / 2;
  const cy = gridY * BLOCK + BLOCK / 2;
  for (let i = 0; i < count; i++) {
    const angle = Math.random() * Math.PI * 2;
    const speed = 1.5 + Math.random() * 4.5;
    particles.push({
      x: cx,
      y: cy,
      vx: Math.cos(angle) * speed,
      vy: Math.sin(angle) * speed - 1.5,
      life: 1.0,
      decay: 0.025 + Math.random() * 0.02,
      size: 3 + Math.random() * 3,
      color
    });
  }
}

function addFloatingText(text, yRow, color = '#ffffff') {
  floatingTexts.push({
    text,
    x: canvas.width / 2,
    y: Math.max(40, yRow * BLOCK),
    vy: -0.9,
    life: 1.0,
    color
  });
}

function explodeBombs(bombPositions) {
  if (bombPositions.length === 0) return;
  AudioEngine.play('bomb');
  triggerShake(260, 7);
  let destroyed = 0;
  bombPositions.forEach(({ bx, by }) => {
    for (let dy = -1; dy <= 1; dy++) {
      for (let dx = -1; dx <= 1; dx++) {
        const nx = bx + dx;
        const ny = by + dy;
        if (nx >= 0 && nx < COLS && ny >= 0 && ny < ROWS && board[ny][nx] !== 0) {
          const color = THEMES[currentTheme].colors[board[ny][nx]] || '#ef4444';
          spawnParticles(nx, ny, color, 8);
          board[ny][nx] = 0;
          destroyed++;
        }
      }
    }
  });
  if (destroyed > 0) {
    score += destroyed * 35;
    addFloatingText(`💣 BOOM! +${destroyed * 35}`, bombPositions[0].by, '#f87171');
  }
}

function mergeAndResolve() {
  const bombs = [];
  player.matrix.forEach((row, y) => {
    row.forEach((value, x) => {
      if (value !== 0 && player.pos.y + y >= 0) {
        const gx = player.pos.x + x;
        const gy = player.pos.y + y;
        board[gy][gx] = value;
        if (value === 8) bombs.push({ bx: gx, by: gy });
      }
    });
  });

  // 폭탄 블록이 포함되어 있다면 착지 즉시 3x3 폭발
  if (bombs.length > 0) {
    explodeBombs(bombs);
  }

  sweepLines();
  checkDangerZone();
  spawnPlayer();
}

function sweepLines() {
  const fullRows = [];
  for (let y = ROWS - 1; y >= 0; y--) {
    // 0이 없는 줄은 완성 (와일드 블록 11 포함)
    if (board[y].every(cell => cell !== 0)) {
      fullRows.push(y);
    }
  }

  if (fullRows.length === 0) {
    combo = -1;
    updateUI();
    return;
  }

  let actuallyRemoved = 0;
  // 아래에서 위로 순회하면서 돌 블록(9)은 균열(10)로 내구도 차감, 나머지는 제거
  fullRows.sort((a, b) => b - a).forEach(y => {
    const hasHardStone = board[y].some(v => v === 9);
    flashRows.push({ y, alpha: 0.9 });

    if (hasHardStone) {
      // 단단한 돌 블록(9)만 금간 돌(10)로 바꾸고 나머지 칸은 비움
      for (let x = 0; x < COLS; x++) {
        const val = board[y][x];
        const color = THEMES[currentTheme].colors[val] || '#ffffff';
        spawnParticles(x, y, color, 5);
        if (val === 9) {
          board[y][x] = 10;
        } else {
          board[y][x] = 0;
        }
      }
      addFloatingText('🪨 ARMOR CRACKED!', y, '#94a3b8');
    } else {
      for (let x = 0; x < COLS; x++) {
        const val = board[y][x];
        const color = THEMES[currentTheme].colors[val] || '#ffffff';
        spawnParticles(x, y, color, 7);
      }
      board.splice(y, 1);
      board.unshift(Array(COLS).fill(0));
      actuallyRemoved++;
    }
  });

  if (actuallyRemoved > 0) {
    AudioEngine.play('clear');
    combo++;
    if (combo > maxCombo) maxCombo = combo;

    const baseTable = [0, 100, 300, 500, 800];
    let gained = (baseTable[actuallyRemoved] || 800) * level;

    // 백투백(4줄 연속) 보너스
    const isTetris = actuallyRemoved === 4;
    if (isTetris) {
      tetrisCount++;
      if (backToBack) {
        gained = Math.floor(gained * 1.5);
        addFloatingText('🔥 BACK-TO-BACK TETRIS!', fullRows[0], '#facc15');
      } else {
        addFloatingText('✨ TETRIS!', fullRows[0], '#22d3ee');
      }
      backToBack = true;
      triggerShake(200, 5);
    } else {
      backToBack = false;
      const labels = ['', 'SINGLE', 'DOUBLE!', 'TRIPLE!!'];
      addFloatingText(`${labels[actuallyRemoved]} +${gained}`, fullRows[0], '#fafafa');
    }

    // 콤보 보너스
    if (combo > 0) {
      const comboBonus = combo * 50 * level;
      gained += comboBonus;
      addFloatingText(`${combo} COMBO! (+${comboBonus})`, fullRows[0] + 1, '#c084fc');
    }

    score += gained;
    linesCleared += actuallyRemoved;
    Storage.data.totalLines += actuallyRemoved;

    // 스킬 게이지 충전
    skillGauge = Math.min(100, skillGauge + actuallyRemoved * 15 + (combo > 0 ? 5 : 0));

    // 배경 펄스 연출
    document.body.classList.add('pulse-bg');
    setTimeout(() => document.body.classList.remove('pulse-bg'), 250);

    // 레벨업 체크 (10줄마다)
    const targetLevel = startLevel + Math.floor(linesCleared / 10);
    if (targetLevel > level) {
      level = targetLevel;
      updateDropSpeed();
      AudioEngine.play('levelup');
      addFloatingText(`⬆ LEVEL ${level}!`, 8, '#4ade80');
    }

    // 40라인 스프린트 클리어 체크
    if (gameMode === 'sprint' && linesCleared >= 40) {
      endGame(true);
    }
  }

  if (score > Storage.data.bestScore) {
    Storage.data.bestScore = score;
  }
  Storage.save();
  updateUI();
}

function updateDropSpeed() {
  const base = Math.max(85, 720 - (level - 1) * 58);
  dropInterval = slowTimer > 0 ? base * 2.2 : base;
}

function checkDangerZone() {
  const isDanger = board.slice(0, 4).some(row => row.some(cell => cell !== 0));
  document.getElementById('board-container').classList.toggle('danger', isDanger);
}

// ==========================================
// 8. 액티브 스킬 & 홀드 & 가비지 라인 (3, 25, 28번)
// ==========================================
function holdPiece() {
  if (!canHold || gameState !== 'playing') return;
  canHold = false;
  AudioEngine.play('rotate');
  if (!holdMatrix) {
    holdMatrix = player.matrix.map(r => [...r]);
    spawnPlayer();
  } else {
    const temp = holdMatrix;
    holdMatrix = player.matrix.map(r => [...r]);
    spawnPlayer(temp);
  }
  drawMiniPreviews();
  updateUI();
}

function useSkillBlast() {
  if (skillGauge < 50 || gameState !== 'playing') return;
  skillGauge -= 50;
  AudioEngine.play('bomb');
  triggerShake(250, 6);

  // 최하단 2줄 제거
  for (let i = 0; i < 2; i++) {
    const y = ROWS - 1;
    for (let x = 0; x < COLS; x++) {
      if (board[y][x] !== 0) {
        spawnParticles(x, y, THEMES[currentTheme].colors[board[y][x]] || '#22d3ee', 6);
      }
    }
    board.pop();
    board.unshift(Array(COLS).fill(0));
  }
  addFloatingText('⚡ LASER BLAST!', ROWS - 2, '#22d3ee');
  checkDangerZone();
  updateUI();
}

function useSkillSlow() {
  if (skillGauge < 50 || gameState !== 'playing') return;
  skillGauge -= 50;
  slowTimer = 5000;
  updateDropSpeed();
  AudioEngine.play('levelup');
  addFloatingText('⏱ TIME SLOW (5s)', 10, '#c084fc');
  updateUI();
}

function pushGarbageLine() {
  const hole = Math.floor(Math.random() * COLS);
  const garbageRow = Array.from({ length: COLS }, (_, x) => (x === hole ? 0 : 12));
  board.shift();
  board.push(garbageRow);
  if (collide(board, player)) {
    player.pos.y--;
    if (player.pos.y < 0) endGame(false);
  }
  triggerShake(150, 4);
  checkDangerZone();
}

// ==========================================
// 9. 이동 및 드롭 제어
// ==========================================
function moveHorizontal(dir) {
  player.pos.x += dir;
  if (collide(board, player)) {
    player.pos.x -= dir;
  } else {
    lockDelayCounter = 0;
    AudioEngine.play('move');
  }
}

function softDrop() {
  player.pos.y++;
  if (collide(board, player)) {
    player.pos.y--;
  } else {
    score += 1;
    dropCounter = 0;
    updateUI();
  }
}

function hardDrop() {
  const startY = player.pos.y;
  const targetY = getGhostY();
  const distance = targetY - startY;

  // 잔상 트레일 기록
  if (distance > 0) {
    dropTrails.push({
      x: player.pos.x,
      fromY: startY,
      toY: targetY,
      matrix: player.matrix.map(r => [...r]),
      alpha: 0.45
    });
  }

  player.pos.y = targetY;
  score += distance * 2;
  AudioEngine.play('drop');
  triggerShake(90, 2.5);
  mergeAndResolve();
  dropCounter = 0;
  updateUI();
}

// ==========================================
// 10. 렌더링 (네온 글로우 블록, 고스트, 파티클, UI)
// ==========================================
function drawStyledBlock(targetCtx, x, y, colorId, size = BLOCK, isGhost = false) {
  const palette = THEMES[currentTheme].colors;
  let color = palette[colorId] || '#22d3ee';

  // 와일드 블록(11)은 시간에 따라 무지개빛 변화
  if (colorId === 11) {
    const hue = (Date.now() / 8) % 360;
    color = `hsl(${hue}, 90%, 65%)`;
  }

  const px = x * size;
  const py = y * size;

  if (isGhost) {
    targetCtx.save();
    targetCtx.strokeStyle = color;
    targetCtx.lineWidth = 1.5;
    targetCtx.setLineDash([4, 3]);
    targetCtx.strokeRect(px + 2, py + 2, size - 4, size - 4);
    targetCtx.fillStyle = color + '22';
    targetCtx.fillRect(px + 2, py + 2, size - 4, size - 4);
    targetCtx.restore();
    return;
  }

  targetCtx.save();
  targetCtx.fillStyle = color;
  targetCtx.shadowColor = color;
  targetCtx.shadowBlur = 8;
  targetCtx.fillRect(px + 1, py + 1, size - 2, size - 2);

  // 상단·좌측 하이라이트 베벨
  targetCtx.shadowBlur = 0;
  targetCtx.fillStyle = 'rgba(255, 255, 255, 0.28)';
  targetCtx.fillRect(px + 2, py + 2, size - 4, 4);
  targetCtx.fillRect(px + 2, py + 2, 4, size - 4);

  // 하단·우측 섀도우 베벨
  targetCtx.fillStyle = 'rgba(0, 0, 0, 0.28)';
  targetCtx.fillRect(px + 2, py + size - 6, size - 4, 4);

  // 특수 블록 아이콘 표시
  if (colorId === 8 || colorId === 9 || colorId === 10 || colorId === 11) {
    targetCtx.font = `${Math.floor(size * 0.55)}px sans-serif`;
    targetCtx.textAlign = 'center';
    targetCtx.textBaseline = 'middle';
    const icon = colorId === 8 ? '💣' : colorId === 9 ? '🪨' : colorId === 10 ? '💥' : '★';
    targetCtx.fillStyle = '#09090b';
    targetCtx.fillText(icon, px + size / 2, py + size / 2 + 1);
  }

  targetCtx.restore();
}

function drawMiniMatrix(targetCtx, cnv, matrix, offsetY = 0) {
  const miniSize = 20;
  const w = matrix[0].length * miniSize;
  const h = matrix.length * miniSize;
  const startX = (cnv.width - w) / 2 / miniSize;
  const startY = offsetY + (70 - h) / 2 / miniSize;

  matrix.forEach((row, y) => {
    row.forEach((val, x) => {
      if (val !== 0) {
        drawStyledBlock(targetCtx, startX + x, startY + y, val, miniSize);
      }
    });
  });
}

function drawMiniPreviews() {
  holdCtx.clearRect(0, 0, holdCanvas.width, holdCanvas.height);
  if (holdMatrix) {
    holdCtx.globalAlpha = canHold ? 1.0 : 0.4;
    drawMiniMatrix(holdCtx, holdCanvas, holdMatrix, 0.2);
    holdCtx.globalAlpha = 1.0;
  }

  nextCtx.clearRect(0, 0, nextCanvas.width, nextCanvas.height);
  nextQueue.forEach((mat, idx) => {
    drawMiniMatrix(nextCtx, nextCanvas, mat, idx * 3.7 + 0.3);
  });
}

function draw() {
  ctx.save();
  ctx.clearRect(0, 0, canvas.width, canvas.height);

  // 화면 흔들림 적용
  if (shakeTime > 0) {
    const dx = (Math.random() - 0.5) * shakeMagnitude;
    const dy = (Math.random() - 0.5) * shakeMagnitude;
    ctx.translate(dx, dy);
  }

  // 1) 10x20 가이드 그리드 (1번)
  ctx.strokeStyle = 'rgba(255, 255, 255, 0.045)';
  ctx.lineWidth = 1;
  for (let x = 1; x < COLS; x++) {
    ctx.beginPath();
    ctx.moveTo(x * BLOCK, 0);
    ctx.lineTo(x * BLOCK, canvas.height);
    ctx.stroke();
  }
  for (let y = 1; y < ROWS; y++) {
    ctx.beginPath();
    ctx.moveTo(0, y * BLOCK);
    ctx.lineTo(canvas.width, y * BLOCK);
    ctx.stroke();
  }

  // 위험 구역(상위 4줄) 점선 경계
  ctx.strokeStyle = 'rgba(239, 68, 68, 0.25)';
  ctx.setLineDash([5, 5]);
  ctx.beginPath();
  ctx.moveTo(0, 4 * BLOCK);
  ctx.lineTo(canvas.width, 4 * BLOCK);
  ctx.stroke();
  ctx.setLineDash([]);

  // 2) 하드드롭 잔상 트레일 (15번)
  dropTrails.forEach(tr => {
    ctx.save();
    ctx.globalAlpha = tr.alpha;
    tr.matrix.forEach((row, y) => {
      row.forEach((val, x) => {
        if (val !== 0) {
          const color = THEMES[currentTheme].colors[val] || '#22d3ee';
          const grad = ctx.createLinearGradient(0, (tr.fromY + y) * BLOCK, 0, (tr.toY + y + 1) * BLOCK);
          grad.addColorStop(0, 'transparent');
          grad.addColorStop(1, color);
          ctx.fillStyle = grad;
          ctx.fillRect((tr.x + x) * BLOCK + 2, (tr.fromY + y) * BLOCK, BLOCK - 4, (tr.toY - tr.fromY + 1) * BLOCK);
        }
      });
    });
    ctx.restore();
  });

  // 3) 고정된 보드 블록
  board.forEach((row, y) => {
    row.forEach((value, x) => {
      if (value !== 0) drawStyledBlock(ctx, x, y, value);
    });
  });

  // 4) 고스트 블록 & 현재 플레이어 블록 (2번)
  if (player.matrix && gameState === 'playing') {
    const ghostY = getGhostY();
    player.matrix.forEach((row, y) => {
      row.forEach((value, x) => {
        if (value !== 0) {
          drawStyledBlock(ctx, player.pos.x + x, ghostY + y, value, BLOCK, true);
        }
      });
    });

    player.matrix.forEach((row, y) => {
      row.forEach((value, x) => {
        if (value !== 0) {
          drawStyledBlock(ctx, player.pos.x + x, player.pos.y + y, value);
        }
      });
    });
  }

  // 5) 라인 클리어 플래시 (12번)
  flashRows.forEach(fr => {
    ctx.fillStyle = `rgba(255, 255, 255, ${fr.alpha})`;
    ctx.fillRect(0, fr.y * BLOCK, canvas.width, BLOCK);
  });

  // 6) 파티클 (13번)
  particles.forEach(p => {
    ctx.save();
    ctx.globalAlpha = Math.max(0, p.life);
    ctx.fillStyle = p.color;
    ctx.fillRect(p.x - p.size / 2, p.y - p.size / 2, p.size, p.size);
    ctx.restore();
  });

  // 7) 부유 텍스트 팝업 (14번)
  floatingTexts.forEach(ft => {
    ctx.save();
    ctx.globalAlpha = Math.max(0, ft.life);
    ctx.font = '800 15px sans-serif';
    ctx.textAlign = 'center';
    ctx.fillStyle = '#000000';
    ctx.fillText(ft.text, ft.x + 1, ft.y + 1);
    ctx.fillStyle = ft.color;
    ctx.fillText(ft.text, ft.x, ft.y);
    ctx.restore();
  });

  ctx.restore();
}

// ==========================================
// 11. 메인 루프 & 타이머
// ==========================================
function formatTime(ms) {
  const totalSec = Math.max(0, ms / 1000);
  const mins = Math.floor(totalSec / 60).toString().padStart(2, '0');
  const secs = Math.floor(totalSec % 60).toString().padStart(2, '0');
  const tenths = Math.floor((ms % 1000) / 100);
  return `${mins}:${secs}.${tenths}`;
}

function updateEffects(deltaTime) {
  if (shakeTime > 0) shakeTime = Math.max(0, shakeTime - deltaTime);

  dropTrails.forEach(t => { t.alpha -= 0.025; });
  dropTrails = dropTrails.filter(t => t.alpha > 0);

  flashRows.forEach(f => { f.alpha -= 0.06; });
  flashRows = flashRows.filter(f => f.alpha > 0);

  particles.forEach(p => {
    p.x += p.vx;
    p.y += p.vy;
    p.vy += 0.15;
    p.life -= p.decay;
  });
  particles = particles.filter(p => p.life > 0);

  floatingTexts.forEach(ft => {
    ft.y += ft.vy;
    ft.life -= 0.018;
  });
  floatingTexts = floatingTexts.filter(ft => ft.life > 0);
}

function update(time = 0) {
  const deltaTime = time - lastTime;
  lastTime = time;

  if (gameState === 'playing') {
    elapsedMs += deltaTime;

    // 울트라(2분 제한) 모드 종료 체크
    if (gameMode === 'ultra' && elapsedMs >= 120000) {
      endGame(true);
    }

    // 슬로우 모션 타이머
    if (slowTimer > 0) {
      slowTimer = Math.max(0, slowTimer - deltaTime);
      if (slowTimer === 0) updateDropSpeed();
    }

    // 가비지 챌린지 모드 (8초마다 방해 줄 상승)
    if (gameMode === 'garbage') {
      garbageTimer += deltaTime;
      const interval = Math.max(4000, 9000 - level * 400);
      if (garbageTimer >= interval) {
        garbageTimer = 0;
        pushGarbageLine();
      }
    }

    // 낙하 및 락 딜레이 처리 (6번)
    player.pos.y++;
    const onGround = collide(board, player);
    player.pos.y--;

    if (onGround) {
      lockDelayCounter += deltaTime;
      if (lockDelayCounter >= LOCK_DELAY_LIMIT) {
        mergeAndResolve();
        dropCounter = 0;
      }
    } else {
      dropCounter += deltaTime;
      if (dropCounter >= dropInterval) {
        player.pos.y++;
        dropCounter = 0;
      }
    }

    // 타이머 UI 갱신
    if (gameMode === 'ultra') {
      document.getElementById('timer-display').textContent = `LEFT: ${formatTime(120000 - elapsedMs)}`;
    } else if (gameMode === 'sprint') {
      document.getElementById('timer-display').textContent = `TIME: ${formatTime(elapsedMs)} (${linesCleared}/40)`;
    } else {
      document.getElementById('timer-display').textContent = `TIME: ${formatTime(elapsedMs)}`;
    }
    document.getElementById('slow-indicator').style.display = slowTimer > 0 ? 'block' : 'none';
  }

  updateEffects(deltaTime);
  draw();
  requestAnimationFrame(update);
}

// ==========================================
// 12. UI 상태 업데이트 & 게임 시작/종료 제어
// ==========================================
function updateUI() {
  document.getElementById('score').textContent = score.toLocaleString();
  document.getElementById('level').textContent = `Lv.${level}`;
  document.getElementById('lines').textContent = `${linesCleared} LINES`;
  document.getElementById('hold-status').textContent = canHold ? 'READY' : 'LOCKED';
  document.getElementById('hold-status').style.color = canHold ? 'var(--accent)' : 'var(--text-dim)';

  document.getElementById('gauge-fill').style.width = `${skillGauge}%`;
  document.getElementById('gauge-text').textContent = `${skillGauge}%`;
  document.getElementById('btn-skill-blast').disabled = skillGauge < 50;
  document.getElementById('btn-skill-slow').disabled = skillGauge < 50;

  const comboBadge = document.getElementById('badge-combo');
  comboBadge.textContent = `COMBO ${Math.max(0, combo)}`;
  comboBadge.classList.toggle('active', combo > 0);

  document.getElementById('badge-b2b').classList.toggle('active', backToBack);
}

function startGame() {
  AudioEngine.init();
  board = createBoard();
  startLevel = parseInt(document.getElementById('start-level').value, 10) || 1;
  level = startLevel;
  score = 0;
  linesCleared = 0;
  combo = -1;
  maxCombo = 0;
  backToBack = false;
  tetrisCount = 0;
  skillGauge = 0;
  slowTimer = 0;
  garbageTimer = 0;
  elapsedMs = 0;
  holdMatrix = null;
  canHold = true;
  particles = [];
  floatingTexts = [];
  dropTrails = [];
  flashRows = [];

  document.getElementById('mode-badge').textContent = gameMode.toUpperCase();
  document.getElementById('board-container').classList.remove('danger');

  updateDropSpeed();
  initNextQueue();
  spawnPlayer();

  Storage.data.totalGames++;
  Storage.save();

  document.getElementById('start-overlay').classList.add('hidden');
  document.getElementById('pause-overlay').classList.add('hidden');
  document.getElementById('gameover-overlay').classList.add('hidden');
  gameState = 'playing';
  updateUI();
}

function endGame(isClear) {
  gameState = 'gameover';
  if (!isClear) AudioEngine.play('gameover');
  else AudioEngine.play('levelup');

  if (gameMode === 'sprint' && isClear) {
    if (!Storage.data.bestSprintMs || elapsedMs < Storage.data.bestSprintMs) {
      Storage.data.bestSprintMs = elapsedMs;
    }
  }
  if (score > Storage.data.bestScore) {
    Storage.data.bestScore = score;
  }
  Storage.save();

  document.getElementById('result-title').textContent = isClear ? 'MODE CLEAR!' : 'GAME OVER';
  document.getElementById('result-subtitle').textContent =
    isClear ? '목표를 달성했습니다! 훌륭한 플레이입니다.' : '블록이 상단에 도달했습니다.';
  document.getElementById('rep-score').textContent = score.toLocaleString();
  document.getElementById('rep-best').textContent = Storage.data.bestScore.toLocaleString();
  document.getElementById('rep-lines').textContent = `Lv.${level} / ${linesCleared}줄`;
  document.getElementById('rep-time').textContent = formatTime(elapsedMs);
  document.getElementById('rep-combo').textContent = `${maxCombo}x / ${tetrisCount}회`;

  document.getElementById('gameover-overlay').classList.remove('hidden');
}

function togglePause() {
  if (gameState === 'playing') {
    gameState = 'paused';
    document.getElementById('pause-overlay').classList.remove('hidden');
  } else if (gameState === 'paused') {
    gameState = 'playing';
    document.getElementById('pause-overlay').classList.add('hidden');
  }
}

// ==========================================
// 13. 이벤트 리스너 바인딩
// ==========================================
document.querySelectorAll('.mode-btn').forEach(btn => {
  btn.addEventListener('click', () => {
    document.querySelectorAll('.mode-btn').forEach(b => b.classList.remove('selected'));
    btn.classList.add('selected');
    gameMode = btn.dataset.mode;
  });
});

document.getElementById('theme-select').addEventListener('change', e => {
  currentTheme = e.target.value;
  const t = THEMES[currentTheme];
  document.documentElement.style.setProperty('--accent', t.accent);
  document.documentElement.style.setProperty('--accent-glow', t.glow);
  drawMiniPreviews();
});

document.getElementById('btn-mute').addEventListener('click', () => {
  AudioEngine.muted = !AudioEngine.muted;
  document.getElementById('btn-mute').textContent = AudioEngine.muted ? '🔇 음소거 [M]' : '🔊 사운드 ON [M]';
});

document.getElementById('btn-pause').addEventListener('click', togglePause);
document.getElementById('btn-resume').addEventListener('click', togglePause);
document.getElementById('btn-start').addEventListener('click', startGame);
document.getElementById('btn-restart').addEventListener('click', startGame);

const showMenu = () => {
  gameState = 'menu';
  document.getElementById('pause-overlay').classList.add('hidden');
  document.getElementById('gameover-overlay').classList.add('hidden');
  document.getElementById('start-overlay').classList.remove('hidden');
};
document.getElementById('btn-to-menu').addEventListener('click', showMenu);
document.getElementById('btn-report-menu').addEventListener('click', showMenu);

document.getElementById('btn-skill-blast').addEventListener('click', useSkillBlast);
document.getElementById('btn-skill-slow').addEventListener('click', useSkillSlow);

document.addEventListener('keydown', event => {
  if (event.code === 'Enter' && gameState === 'menu') {
    startGame();
    return;
  }
  if (event.key.toLowerCase() === 'm') {
    document.getElementById('btn-mute').click();
    return;
  }
  if (event.key.toLowerCase() === 'p' || event.key === 'Escape') {
    if (gameState === 'playing' || gameState === 'paused') togglePause();
    return;
  }
  if (event.key.toLowerCase() === 'r' && (gameState === 'playing' || gameState === 'gameover')) {
    startGame();
    return;
  }

  if (gameState !== 'playing') return;

  if (event.key === 'ArrowLeft') {
    moveHorizontal(-1);
  } else if (event.key === 'ArrowRight') {
    moveHorizontal(1);
  } else if (event.key === 'ArrowDown') {
    softDrop();
  } else if (event.key === 'ArrowUp') {
    tryRotate();
  } else if (event.code === 'Space') {
    event.preventDefault();
    hardDrop();
  } else if (event.key.toLowerCase() === 'c' || event.key === 'Shift') {
    holdPiece();
  } else if (event.key.toLowerCase() === 'x') {
    useSkillBlast();
  } else if (event.key.toLowerCase() === 'z') {
    useSkillSlow();
  }
});

// 초기화
Storage.load();
requestAnimationFrame(update);