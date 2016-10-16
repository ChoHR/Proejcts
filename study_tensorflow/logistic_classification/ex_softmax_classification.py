import tensorflow as tf
import numpy as np

xy = np.loadtxt('train_softmax_classification.txt', unpack=True, dtype='float32')
x_data = np.transpose(xy[0:4])  # 4 x 1 -> 1 x 4 matrix
y_data = np.transpose(xy[4:])   # 3 x 1 -> 1 x 3 matrix

w = tf.Variable(tf.zeros([4, 3])) # 4 x 3

X = tf.placeholder("float", [None, 4])
Y = tf.placeholder("float", [None, 3])
h = tf.matmul(X, w)

hypothesis = tf.nn.softmax(h)
                
cost = tf.reduce_mean(-tf.reduce_sum(Y*tf.log(hypothesis), reduction_indices=1))

a = tf.Variable(0.01) # Learning rate, alpha
optimizer = tf.train.GradientDescentOptimizer(a)
train = optimizer.minimize(cost)

init = tf.initialize_all_variables()

#with tf.Session() as sess:
#    sess.run(init)
    
#    for step in xrange(2001):
#        sess.run(train, feed_dict={X:x_data, Y:y_data})
#        if step % 200 == 0:
#            print step, run(cost,  feed_dict={X:x_data, Y:y_data}), sess.run(w)

sess = tf.Session()
sess.run(init)

for step in xrange(2001):
    sess.run(train, feed_dict={X:x_data, Y:y_data})
    if step % 200 == 0:
        print step, sess.run(cost, feed_dict={X:x_data, Y:y_data}), sess.run(w)

# Learns best fit is W: [1], b: [0]
#print sess.run(hypothesis, feed_dict={X:[[1], [2], [2]]})>0.5