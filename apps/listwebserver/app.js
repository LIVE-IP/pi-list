const express = require("express");
const cookieParser = require('cookie-parser');
const session = require('express-session');
const morgan = require("morgan");
const helmet = require('helmet');
const cors = require('cors');
const bodyParser = require("body-parser");
const { promisify } = require("util");
const child_process = require("child_process");
const api = require("./api");
const auth = require('./auth');
const { apiErrorHandler, resourceNotFoundHandler, isAuthenticated } = require('./util/express-middleware');
const program = require("./util/programArguments");
const logger = require('./util/logger');

const app = express();

// Initialize the REST API logger
app.use(morgan('short', { stream: logger('rest-api').restAPILogger }));

// User custom middleware in order to set the Access-Control-Allow-Credentials as true.
app.use(cors({
    origin: program.webappDomain,
    credentials: true
}));

//Helmet protects the express web servers from some well-known web vulnerabilities
app.use(helmet());

app.use(bodyParser.json());

app.use(cookieParser());

app.use(session({
    secret: program.cookieSecret,
    resave: true,
    saveUninitialized: true,
    rolling: true,
    cookie: {
        maxAge: 30 * 60 * 1000 //30 minutes (in milliseconds)
    }
}));

auth(app);

// API Router
app.use('/api/', isAuthenticated, api);

// Handles with the 404 Not Found
app.use(resourceNotFoundHandler);

// Handles API Error and send a specific error code for the API Rest Consumer instead of the
// stack trace message. Those kind of information should propagated to the logger.
app.use(apiErrorHandler);


// Generate static config data when the LIST web server is executed.
const generateStaticConfigCommand = `"${program.cpp}/static_generator" "${program.folder}"`;

logger('static-generator').profile('Static configurations generated');

// Promisify execute command
const exec = promisify(child_process.exec);
exec(generateStaticConfigCommand)
    .then((output) => {
        logger('static-generator').info(`Generated static configurations: ${output.stdout}`);
    })
    .catch((output) => {
        logger('static-generator').error(output.stderr);
    })
    .then(() => {
        logger('static-generator').profile('Static configurations generated');
    });

module.exports = app;
