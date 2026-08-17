const express = require('express');
const path = require('path');
const hbs = require('hbs');

require('./app_server/models/db');

const routes = require('./app_server/routes/index');
const apiRoutes = require('./app_api/routes/index');

const app = express();

app.use((req, res, next) => {
  res.header('Access-Control-Allow-Origin', '*');
  res.header(
    'Access-Control-Allow-Headers',
    'Origin, X-Requested-With, Content-Type, Accept'
  );
  res.header(
    'Access-Control-Allow-Methods',
    'GET, POST, PUT, DELETE, OPTIONS'
  );
  next();
});

app.set('views', path.join(__dirname, 'app_server', 'views'));
app.set('view engine', 'hbs');

app.use(express.json());
app.use(express.urlencoded({ extended: false }));

app.use(express.static(path.join(__dirname)));

app.use('/', routes);
app.use('/api', apiRoutes);

module.exports = app;