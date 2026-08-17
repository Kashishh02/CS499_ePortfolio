const express = require('express');
const router = express.Router();

const ctrlTravel = require('../controllers/travel');

router.get('/travel', ctrlTravel.travel);

module.exports = router; 