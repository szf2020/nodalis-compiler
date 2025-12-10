import fs from 'fs';
import path from 'path';
import { MTIProgrammer } from '../../src/programmers/MTIProgrammer.js';

var fixtureName = 'FF-1';
var inputPath = path.resolve('test/skip/output', `${fixtureName}.xml`);


function normalize(text) {
  return text.replace(/\s+/g, ' ').trim();
}

function runTest() {
  
  let programmer = new MTIProgrammer({
    source: inputPath,
    destination: "192.168.9.15",
    target: 'MTI'
  });
  programmer.program();

}

runTest();