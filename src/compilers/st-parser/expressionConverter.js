/* eslint-disable curly */
/* eslint-disable eqeqeq */
// Copyright [2025] Nathan Skipper
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


export function convertExpression(expr) {
  if (Array.isArray(expr)) expr = expr.join(" ");

  var results = expr
    .replace(/\bAND\b/gi, '&&')
    .replace(/\bOR\b/gi, '||')
    .replace(/\bNOT\b/gi, '!')
    .replace(/\bMOD\b/gi, '%')
    .replace(/\bDIV\b/gi, '/')
    .replace(/<>/g, '!=')
    .replace(/:=/g, '=')
    .replace(/\bTRUE\b/gi, 'true')
    .replace(/\bFALSE\b/gi, 'false')
    .replace(/(?<![=!<>])=(?![=])/g, '==');

  // Replace %I/Q/M references with readAddress(...) before anything else
  const parts = results.split(/\s+/);
  results = parts.map(e => {
    return /^%[IQM]\d+(\.\d+)?$/i.test(e) ? `readAddress("${e}")` : e;
  }).join(' ');

  // Now replace var.bit (but NOT %I0001.0) with getBit(...)
  if(results.indexOf("readAddress") === -1){
    results = results.replace(/\b(?!%)(([A-Za-z_]\w*)\.(\d+))\b/g, (_, full, base, bit) => {
        return `getBit(${base}, ${bit})`;
    });
  }
  return results;
}

