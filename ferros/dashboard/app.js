const CONFIG = {
    INDEX_URL: '../JSON/index.json',
    LATEST_URL: '../JSON/latest.json',
    POLL_INTERVAL: 3000, // 3 seconds
};

let processData = new Map(); // PID -> ProcessInsight
let filterQuery = '';
let sortBy = 'event_count';

async function fetchNDJSON(url) {
    try {
        const response = await fetch(url);
        if (!response.ok) return [];
        const text = await response.text();
        return text.split('\n')
            .filter(line => line.trim())
            .map(line => JSON.parse(line));
    } catch (e) {
        console.error(`Error fetching NDJSON from ${url}:`, e);
        return [];
    }
}

async function fetchJSON(url) {
    try {
        const response = await fetch(url);
        if (!response.ok) return null;
        return await response.json();
    } catch (e) {
        console.error(`Error fetching JSON from ${url}:`, e);
        return null;
    }
}

function updateProcessMap(records) {
    records.forEach(record => {
        const existing = processData.get(record.pid);
        if (!existing || record.last_seen > existing.last_seen) {
            processData.set(record.pid, record);
        }
    });
}

async function loadHistory() {
    const index = await fetchJSON(CONFIG.INDEX_URL);
    if (!index || !Array.isArray(index)) return;

    // Load all historical files
    const loadPromises = index.map(filename => fetchNDJSON(`../JSON/${filename}`));
    const results = await Promise.all(loadPromises);
    
    results.forEach(records => updateProcessMap(records));
    render();
}

async function pollLatest() {
    const latest = await fetchJSON(CONFIG.LATEST_URL);
    if (latest && Array.isArray(latest)) {
        updateProcessMap(latest);
        document.getElementById('refresh-time').textContent = new Date().toLocaleTimeString();
        render();
    }
}

function formatTime(ns) {
    if (!ns) return 'N/A';
    // Assuming ns is a large timestamp or duration. 
    // If it's a relative kernel time, we might just show the raw value or convert to something readable.
    // For now, let's just show it as a string or a simplified number.
    return (ns / 1e9).toFixed(2) + 's';
}

function render() {
    const tableBody = document.getElementById('table-body');
    const statTotal = document.getElementById('stat-total');
    const statHigh = document.getElementById('stat-high');
    const statEvents = document.getElementById('stat-events');

    let dataArray = Array.from(processData.values());

    // Filter
    if (filterQuery) {
        const query = filterQuery.toLowerCase();
        dataArray = dataArray.filter(p => 
            p.comm.toLowerCase().includes(query) || 
            p.pid.toString().includes(query)
        );
    }

    // Sort
    dataArray.sort((a, b) => {
        if (sortBy === 'event_count') return b.event_count - a.event_count;
        if (sortBy === 'pid') return a.pid - b.pid;
        if (sortBy === 'comm') return a.comm.localeCompare(b.comm);
        return 0;
    });

    // Update Stats
    statTotal.textContent = processData.size;
    statHigh.textContent = Array.from(processData.values()).filter(p => p.event_count > 100).length;
    statEvents.textContent = Array.from(processData.values()).reduce((sum, p) => sum + p.event_count, 0);

    // Render Table
    tableBody.innerHTML = '';
    dataArray.forEach(p => {
        const row = document.createElement('tr');
        
        const isHigh = p.event_count > 100;
        const statusClass = p.exited ? 'exited' : 'running';
        const statusText = p.exited ? 'Exited' : 'Running';

        row.innerHTML = `
            <td class="pid-cell">${p.pid}</td>
            <td class="comm-cell">${p.comm}</td>
            <td class="count-cell ${isHigh ? 'high-count' : ''}">${p.event_count}</td>
            <td class="time-cell">${formatTime(p.first_seen)}</td>
            <td class="time-cell">${formatTime(p.last_seen)}</td>
            <td><span class="status-badge ${statusClass}">${statusText}</span></td>
        `;
        tableBody.appendChild(row);
    });
}

// Event Listeners
document.getElementById('filter-input').addEventListener('input', (e) => {
    filterQuery = e.target.value;
    render();
});

document.getElementById('sort-select').addEventListener('change', (e) => {
    sortBy = e.target.value;
    render();
});

// Initialization
async function init() {
    await loadHistory();
    setInterval(pollLatest, CONFIG.POLL_INTERVAL);
    // Initial poll
    pollLatest();
}

init();
