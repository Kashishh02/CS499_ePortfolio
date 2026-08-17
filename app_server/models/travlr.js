const mongoose = require('mongoose');

const tripSchema = new mongoose.Schema({
    code: {
        type: String,
        required: [true, 'Trip code is required'],
        unique: true,
        trim: true
    },
    name: {
        type: String,
        required: [true, 'Trip name is required'],
        trim: true,
        minlength: 3
    },
    length: {
        type: String,
        required: [true, 'Trip length is required'],
        trim: true
    },
    start: {
        type: Date,
        required: [true, 'Start date is required'],
        validate: {
            validator: function(value) {
                return value instanceof Date && !isNaN(value.getTime());
            },
            message: 'Start date must be a valid date'
        }
    },
    resort: {
        type: String,
        required: [true, 'Resort name is required'],
        trim: true,
        minlength: 2
    },
    perPerson: {
        type: Number,
        required: [true, 'Price is required'],
        min: [0, 'Price cannot be negative']
    },
    image: {
        type: String,
        required: [true, 'Image is required'],
        trim: true
    },
    description: {
        type: String,
        required: [true, 'Description is required'],
        trim: true,
        minlength: 10
    }
});

mongoose.model('trips', tripSchema);