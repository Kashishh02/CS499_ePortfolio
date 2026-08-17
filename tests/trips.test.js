const request = require('supertest');
const app = require('../app');

describe('Trip API Validation Tests', () => {

    test('POST /api/trips returns 400 for invalid trip data', async () => {
        const invalidTrip = {
            code: 'TEST',
            name: 'Test Trip',
            length: '5 days',
            start: 'not-a-valid-date',
            resort: 'Test Resort',
            perPerson: -100,
            image: 'test.jpg',
            description: 'This is a test trip description.'
        };

        const response = await request(app)
            .post('/api/trips')
            .send(invalidTrip);

        expect(response.statusCode).toBe(400);
        expect(response.body.message).toBe('Unable to create trip.');
    });

    test('PUT /api/trips/:tripId returns 400 for invalid trip data', async () => {
        const invalidUpdate = {
            perPerson: -50
        };

        const response = await request(app)
            .put('/api/trips/000000000000000000000000')
            .send(invalidUpdate);

        expect(response.statusCode).toBe(400);
        expect(response.body.message).toBe('Unable to update trip.');
    });

});