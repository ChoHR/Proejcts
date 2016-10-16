import tensorflow as tf
import numpy as np

xy = np.loadtxt('train.txt', unpack=True, dtype='float32')
x_data = xy[0:-1]
y_data = xy[-1];

print 'x', x_data
print 'y', y_data

w = tf.Variable(tf.random_uniform([1,len(x_data)], -5.0, 5.0))

X = tf.placeholder(tf.float32)
Y = tf.placeholder(tf.float32)

hypothesis = tf.matmul(w, X)
                
cost = tf.reduce_mean(tf.square(hypothesis - Y))

a = tf.Variable(0.1) # Learning rate, alpha
optimizer = tf.train.GradientDescentOptimizer(a)
train = optimizer.minimize(cost)

init = tf.initialize_all_variables()

sess = tf.Session()
sess.run(init)

for step in xrange(1001):
    sess.run(train, feed_dict={X:x_data, Y:y_data})
    if step % 20 == 0:
        print step, sess.run(cost, feed_dict={X:x_data, Y:y_data}), sess.run(w)

# Learns best fit is W: [1], b: [0]
#print sess.run(hypothesis, feed_dict={X1:5, X2:4})
#print sess.run(hypothesis, feed_dict={X1:2.5, X2:1.5})