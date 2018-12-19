var path = require('path');
var webpack = require('webpack');
var vtkRules = require('vtk.js/Utilities/config/dependency.js').webpack.core.rules;

// Optional if you want to load *.css and *.module.css files
// var cssRules = require('vtk.js/Utilities/config/dependency.js').webpack.css.rules; 

var entry = path.join(__dirname, './src/index.js');
const sourcePath = path.join(__dirname, './src');
const outputPath = path.join(__dirname, './dist');

module.exports = {
    entry,
    output: {
        path: outputPath,
        filename: 'MyWebApp.js',
    },
    module: {
        rules: [
            {
                test: /\.html$/,
                loader: 'html-loader'
            },
    ].concat(vtkRules),
    },
    resolve: {
        modules: [
      path.resolve(__dirname, 'node_modules'),
      sourcePath,
    ],
    },
    node: {
        fs: 'empty',
        net: 'empty'
    },
};
