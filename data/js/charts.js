// /js/charts.js
import { state } from './state.js';
import { $, appendLog } from './dom.js';

const MAX_POINTS = 300;

export function initCharts(){
  const mk = (canvas, labels) => {
    if (!window.Chart){
      canvas.parentElement.innerHTML = '<div class="readout">Chart.js 未加载，图表不可用</div>'; 
      return null;
    }
    return new Chart(canvas, {
      type: 'line',
      data: {
        labels: Array.from({length: MAX_POINTS}, (_, i) => i),
        datasets: labels.map(l => ({ label: l, data: Array(MAX_POINTS).fill(0), borderWidth: 1, tension: .15, pointRadius: 0 }))
      },
      options: {
        animation:false, responsive:true, maintainAspectRatio:false,
        scales:{ x:{display:false}, y:{beginAtZero:true} },
        plugins:{ legend:{ display:true, position:'bottom', labels:{ boxWidth:10 } } }
      }
    });
  };
  state.charts.ch1 = mk($('#chart1'), ['epos','espd','eang']);
  state.charts.ch2 = mk($('#chart2'), ['wspd1','wspd2','wspd']);
  state.charts.ch3 = mk($('#chart3'), ['tbase','tyaw','tvel']);
  appendLog('[INIT] charts ready');
}

function push3(chart, a,b,c){
  if (!chart) return;
  const ds = chart.data.datasets;
  [a,b,c].forEach((v,i) => { ds[i].data.push(v); if (ds[i].data.length > MAX_POINTS) ds[i].data.shift(); });
  chart.update('none');
}

export function feedCharts(a1,a2,a3, b1,b2,b3, c1,c2,c3){
  push3(state.charts.ch1, a1,a2,a3);
  push3(state.charts.ch2, b1,b2,b3);
  push3(state.charts.ch3, c1,c2,c3);
}
