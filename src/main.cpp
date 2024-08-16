#include <cmath>
#include <iostream>
#include <SFML/Graphics.hpp>

const int kWindowWidthPx = 512;
const int kWindowHeightPx = 512;
const int kSeed = 0;
const int kGridStep = 32;
const int kModulo = 256;
const int kDimension = 2;
const double kAmplitude = 1.0;
const double kFrequency = 0.05;
const int kNumLayers = 5;
const double kRateOfChange = 2.0;

void Normalize(std::vector<double>& v) {
    double len = std::sqrt(v[0] * v[0] + v[1] * v[1]);
    v[0] /= len;
    v[1] /= len;
}

// generates array of gradient vectors (x, y) with random values from interval [-1.0, 1.0]
// and normalizes them
std::array<std::vector<double>, 256> PrecomputeGradients() {
    std::array<std::vector<double>, 256> res;
    for (int i = 0; i < 256; ++i) {
        double x = ((rand() % (2 * kModulo)) - kModulo) / static_cast<double>(kModulo);
        double y = ((rand() % (2 * kModulo)) - kModulo) / static_cast<double>(kModulo);
        std::vector<double> grad = {x, y};
        Normalize(grad);
        res[i] = grad;
    }
    return res;
}


// generates array ranodom of noise values from the interval [0.0, 1.0]
std::array<double, 256> PrecomputeNoise() {
    std::array<double, 256> res;
    for (int i = 0; i < 256; ++i) {
        res[i] = (rand() % kModulo) / static_cast<double>(kModulo);
    }
    return res;
}

// generates a random permutation of array of numbers [0, 1, 2, ..., 255]
std::array<int, 256> GenRandomPermutation() {
    std::array<int, 256> res;
    for (int i = 0; i < 256; ++i) {
        res[i] = i;
    }
    std::random_shuffle(res.begin(), res.end());
    return res;
}

// shuffles the array
void UpdateIndexPermutation(std::array<int, 256>& index_permutation) {
    std::random_shuffle(index_permutation.begin(), index_permutation.end());
}

// linear interpolation of values a0 and a1 with coefficients w and (1 - w)
double Lerp(double a0, double a1, double w) {
    return (1.0 - w) * a0 + w * a1;
}


double Smoothstep(double x) {
    return x * x * x * (10 + x * (-15 + 6 * x));
    //return x * x * (3 - 2 * x);
    //return x;
}

double PerlinNoise(int x, int y, const std::array<std::vector<double>, 256>& precomputed_gradients,
            const std::array<int, 256> &index_permutation, int grid_step) {
    double x_norm = static_cast<double>(x) / grid_step;
    double y_norm = static_cast<double>(y) / grid_step;
    x_norm -= static_cast<int>(x_norm);
    y_norm -= static_cast<int>(y_norm);
    int x_grid_top_left = (x / grid_step) * grid_step;//x coordinate of top left grid neighbor of (x, y)
    int y_grid_top_left = (y / grid_step) * grid_step;
    //gradient indices of grid neighbors of (x, y)
    int gradient_index0 = index_permutation[(x_grid_top_left + index_permutation[y_grid_top_left % 256]) % 256];
    int gradient_index1 = index_permutation[(x_grid_top_left + 1 + index_permutation[y_grid_top_left % 256]) % 256];
    int gradient_index2 = index_permutation[(x + index_permutation[(y_grid_top_left + 1) % 256]) % 256];
    int gradient_index3 = index_permutation[(x_grid_top_left + 1 + index_permutation[(y_grid_top_left + 1) % 256]) % 256];

    double dot_product0 = precomputed_gradients[gradient_index0][0] * x_norm + precomputed_gradients[gradient_index0][1] * y_norm;
    double dot_product1 = precomputed_gradients[gradient_index1][0] * (x_norm - 1) + precomputed_gradients[gradient_index1][1] * y_norm;
    double dot_product2 = precomputed_gradients[gradient_index2][0] * x_norm + precomputed_gradients[gradient_index2][1] * (y_norm - 1);
    double dot_product3 = precomputed_gradients[gradient_index3][0] * (x_norm - 1) + precomputed_gradients[gradient_index3][1] * (y_norm - 1);

    double Lerp01 = Lerp(dot_product0, dot_product1, Smoothstep(x_norm));
    double Lerp23 = Lerp(dot_product2, dot_product3, Smoothstep(x_norm));
    double res = Lerp(Lerp01, Lerp23, Smoothstep(y_norm));
    
	return (res + 1) / 2;
}


double GenWhiteNoise(double x, double y) {
    return (rand() % kModulo) / static_cast<double>(kModulo);
}

double GenSimpleNoise(double x, double y, const std::array<double, 256>& precomputed_noise,
                        const std::array<int, 256>& index_permutation, int grid_step) {
    int x_floor = std::floor(x);
    int y_floor = std::floor(y);

    int x0 = x_floor;
    int x1 = x0 + 1;
    int y0 = y_floor;
    int y1 = y0 + 1;

    double noise00 = precomputed_noise[(index_permutation[x0 % 256] + y0) % 256];
    double noise10 = precomputed_noise[(index_permutation[x1 % 256] + y0) % 256];
    double noise01 = precomputed_noise[(index_permutation[x0 % 256] + y1) % 256];
    double noise11 = precomputed_noise[(index_permutation[x1 % 256] + y1) % 256];

    double x_frac = x - x_floor;
    double y_frac = y - y_floor;

    double weight_x = Smoothstep(x_frac);
    double weight_y = Smoothstep(y_frac);


    double Lerp_horiz_top = Lerp(noise00, noise10, weight_x);
    double Lerp_horiz_bottom = Lerp(noise01, noise11, weight_x);
    double Lerp_vertical = Lerp(Lerp_horiz_top, Lerp_horiz_bottom, weight_y);

   return Lerp_vertical;
}

// converts number from the interval [0.0, 1.0] into number from the interval [-1.0, 1.0]
double NoiseUnsignedToSigned(double x) {
    return 2 * x - 1;
    //return cos(acos(-1) * x);
    //return atan(acos(-1) / 4 * (1 - 2 * x));
}

int main() {
    srand(time(nullptr));
    sf::RenderWindow window(sf::VideoMode(kWindowWidthPx, kWindowHeightPx), "PerlinNoise");

    window.setFramerateLimit(6);

    sf::Image image;
    image.create(kWindowWidthPx, kWindowHeightPx, sf::Color::Blue);

    sf::Texture texture;
    texture.loadFromImage(image, sf::IntRect(0, 0, kWindowWidthPx, kWindowHeightPx));
    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(0, 0, kWindowWidthPx, kWindowHeightPx));
    sf::Uint8 *pixels = new sf::Uint8[kWindowWidthPx * kWindowHeightPx * 4];
    double *pixels_double = new double[kWindowWidthPx * kWindowHeightPx * 4];
    std::fill(pixels, pixels + kWindowWidthPx * kWindowHeightPx * 4, 255);
    std::fill(pixels_double, pixels_double + kWindowWidthPx * kWindowHeightPx * 4, 255);


    std::array<double, 256> precomputed_noise = PrecomputeNoise();
    std::array<std::vector<double>, 256> precomputed_gradients = PrecomputeGradients();
    std::array<int, 256> index_permutation = GenRandomPermutation();


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        UpdateIndexPermutation(index_permutation);
        double max_noise_val = 0.0;
        for (int y = 0; y < kWindowHeightPx; ++y) {
            for (int x = 0; x < kWindowWidthPx; ++x) {

                double sum_noise = 0;
                double frequency = 0.02;
                double frequency_multiplier = 1.8;//lacunarity
                double amplitude_multiplier = 0.35;
                //double frequency_multiplier = 2;//lacunarity
                //double amplitude_multiplier = 0.5;
                double x_temp = x * frequency;
                double y_temp = y * frequency;
                double amplitude = 1.0;
                double temp = 1.0;
                double tot = 0.0;
                int num_layers = 5;
                for (int k = 0; k < num_layers; ++k) {
                    double curr_noise = GenSimpleNoise(x_temp * temp, y_temp * temp, 
                        precomputed_noise, index_permutation, kGridStep) * amplitude;
                    //curr_noise = fabs(NoiseUnsignedToSigned(curr_noise));
                    sum_noise += curr_noise;
                    tot += curr_noise * curr_noise;
                    max_noise_val = std::max(max_noise_val, curr_noise);
                    amplitude *= amplitude_multiplier;
                    temp *= frequency_multiplier;
                }
                sum_noise *= 8;//wood texture
                //sum_noise = (sin((x + sum_noise * 400) * 2 * acos(-1) / 100.0) + 1) / 2.0;
                //sum_noise /= tot;
                //sum_noise /= sqrt(tot);
                for (int k = 0; k < 3; ++k) {
                    pixels[(x + kWindowWidthPx * y) * 4 + k] = 255 * sum_noise;
                }
            }
        }

        texture.update(pixels);
        sprite.setTexture(texture);
        sprite.setTextureRect(sf::IntRect(0, 0, kWindowWidthPx, kWindowHeightPx));


        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }
    return 0;
}


