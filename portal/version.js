const { spawn } = require('child_process');
var fs = require('fs');

const gitDescribe = spawn('git', ['describe', '--dirty']);

gitDescribe.stdout.on('data', (data) => {
  var fileContent = `export const Version = '${(data + "").trim()}';
`;
  fs.writeFile('src/version.ts', fileContent, function (err) {
    if (err) throw err;
  });
});
