const mongoose = require('mongoose');

const User = mongoose.model('users');

const login = async (req, res) => {
    const { email } = req.body;

    try {
        const user = await User.findOne({ email });

        if (!user) {
            return res.status(401).json({
                message: 'Invalid credentials'
            });
        }

        res.status(200).json({
            token: 'mock-jwt-token'
        });
    } catch (err) {
        res.status(500).json(err);
    }
};

module.exports = {
    login
};