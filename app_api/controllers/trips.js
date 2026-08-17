const mongoose = require('mongoose');
const Trip = mongoose.model('trips');

const tripsList = async (req, res) => {
    try {
        const trips = await Trip.find({});
        res.status(200).json(trips);
     } catch (err) {
            res.status(500).json({
                message: "An error occurred while retrieving trips.",
                error: err.message
            });
        }
    };
const tripsFindByCode = async (req, res) => {
    try {
        const trip = await Trip.findOne({
            code: req.params.tripCode
        });

        if (!trip) {
            return res.status(404).json({
                message: 'Trip not found'
            });
        }

        res.status(200).json(trip);
    } catch (err) {
        res.status(500).json({
            message: "An error occurred while retrieving the trip.",
            error: err.message
        });
    }
};

const tripsAddTrip = async (req, res) => {
    try {
        const trip = await Trip.create(req.body);
        res.status(201).json(trip);
    } catch (err) {
        res.status(400).json({
            message: "Unable to create trip.",
            error: err.message
        });
    }
};

const tripsUpdateTrip = async (req, res) => {
    try {
        const trip = await Trip.findByIdAndUpdate(
            req.params.tripId,
            req.body,
            {
                new: true,
                runValidators: true
            }
        );

        res.status(200).json(trip);
    } catch (err) {
        res.status(400).json({
            message: "Unable to update trip.",
            error: err.message
        });
    }
};

const tripsDeleteTrip = async (req, res) => {
    try {
        await Trip.findByIdAndDelete(req.params.tripId);
        res.status(204).json(null);
    } catch (err) {
        res.status(400).json({
            message: "Unable to delete trip.",
            error: err.message
        });
    }
};

module.exports = {
    tripsList,
    tripsFindByCode,
    tripsAddTrip,
    tripsUpdateTrip,
    tripsDeleteTrip
};